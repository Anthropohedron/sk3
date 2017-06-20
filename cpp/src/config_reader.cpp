#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "yaml-cpp/yaml.h"
#include "config_reader.hpp"
#include "event_queue.hpp"
#include "task.hpp"
#include "machine.hpp"
#include "demand.hpp"

template<class T> inline
void setNames(std::map<std::string, T> dict) {
  std::for_each(dict.begin(), dict.end(),
      [](std::pair<const std::string, T> &val) {
        val.second.name = val.first;
      });
}

namespace YAML {

bool convert<SK3C::Logger>::decode(const Node &node, SK3C::Logger &rhs) {
  Node file  = node["file"];
  Node dir   = node["dir"];
  Node limit = node["limit"];
  rhs.file  =  file.IsScalar() ?  file.as<std::string>() : "";
  rhs.dir   =   dir.IsScalar() ?   dir.as<std::string>() : "";
  rhs.limit = limit.IsScalar() ? limit.as<long>()        :  0;
  return true;
}

bool convert<SK3C::Variants>::decode(const Node &node, SK3C::Variants &rhs) {
  if (node.IsMap()) {
    rhs.task    = "Task";
    rhs.machine = "Machine";
    rhs.demand  = "Demand";
  } else {
    Node task    = node["task"];
    Node machine = node["machine"];
    Node demand  = node["demand"];
    rhs.task    =    task.IsScalar() ?    task.as<std::string>() : "Task";
    rhs.machine = machine.IsScalar() ? machine.as<std::string>() : "Machine";
    rhs.demand  =  demand.IsScalar() ?  demand.as<std::string>() : "Demand";
  }
  return true;
}

bool convert<SK3C::Task>::decode(const Node &node, SK3C::Task &rhs) {
  if (!node.IsMap()) return false;
  Node batch_size = node["batch_size"];
  Node batch_time = node["batch_time"];
  Node unit_time  = node["unit_time"];
  Node deficit    = node["deficit"];
  Node debit      = node["debit"];
  if (!(batch_size.IsScalar() &&
        (batch_time.IsScalar() || unit_time.IsScalar()))) {
    return false;
  }
  rhs.batch_size = batch_size.as<double>();
  rhs.batch_time = batch_time.IsScalar() ? batch_time.as<double>() :
    unit_time.as<double>() * rhs.batch_size;
  rhs.deficit = deficit.IsScalar() ? deficit.as<double>() :
    debit.IsScalar() ? debit.as<double>() : 0;
  return true;
}

bool convert<SK3C::Machine>::decode(const Node &node, SK3C::Machine &rhs) {
  if (!node.IsSequence()) return false;
  rhs.tasks = node.as<std::vector<std::string>>();
  return true;
}

bool convert<SK3C::Demand>::decode(const Node &node, SK3C::Demand &rhs) {
  if (!node.IsMap()) return false;
  Node quantity    = node["quantity"];
  Node interval    = node["interval"];
  Node destination = node["destination"];
  Node offset      = node["offset"];
  if (!quantity.IsScalar()) {
    return false;
  }
  if (interval.IsScalar()) {
    if (destination.IsScalar() &&
        (destination.as<std::string>() != "external")) {
      return false;
    }
    rhs.interval = interval.as<double>();
    rhs.destination = "";
  } else if (destination.IsScalar()) {
    rhs.interval = 0;
    rhs.destination = destination.as<std::string>();
  } else {
    return false;
  }
  rhs.quantity = quantity.as<double>();
  rhs.offset = offset.IsScalar() ? offset.as<double>() : 0;
  return true;
}

template<class F, class S> inline
const S &getSecond(const std::pair<F, S> &p) {
  return p.second;
}

bool convert<SK3C::Config>::decode(const Node &node, SK3C::Config &rhs) {
  if (!node.IsMap()) return false;
  rhs.logger = node["logger"].as<SK3C::Logger>();
  rhs.variants = node["variants"].as<SK3C::Variants>();
  rhs.tasks = node["tasks"].as<std::map<std::string, SK3C::Task>>();
  setNames(rhs.tasks);
  rhs.machines = node["machines"].as<std::map<std::string, SK3C::Machine>>();
  setNames(rhs.machines);
  rhs.demands = node["demands"].as<std::map<std::string, SK3C::Demand>>();
  setNames(rhs.demands);
  return true;
}

}

namespace SK3 {
namespace Config {

using namespace std;

namespace Validate {

static inline void err(const Task &task, const string &msg) {
  cerr << "ERROR: Task " << task.name << msg << endl;
}

int CEImpl<Task>::countErrors(const Task &task) {
  int errors = 0;
  if (task.batch_size <= 0) {
    err(task, " has non-positive batch_size");
    ++errors;
  }
  if (task.batch_time < 0) {
    err(task, " has negative batch_time");
    ++errors;
  }
  return errors;
}

static inline void err(const Machine &machine, const string &msg,
    const string &msg2 = "",
    const string &msg3 = "",
    const string &msg4 = "") {
  cerr << "ERROR: Machine " << machine.name
    << msg << msg2 << msg3 << msg4 << endl;
}

int CEImpl<Machine>::countErrors(const Machine &machine) {
  int errors = 0;
  if (machine.tasks.empty()) {
    err(machine, " has no assigned tasks");
    ++errors;
  } else {
    map<string, string> &assigned = assignedTasks;
    const map<string, Task> &known = knownTasks;
    for_each(machine.tasks.begin(), machine.tasks.end(),
        [&errors,&assigned,&known,&machine](const string &task) {
          if (known.find(task) == known.end()) {
            err(machine, " has unknown task ", task, " assigned");
            ++errors;
          } else {
            auto found = assigned.find(task);
            if (found == assigned.end()) {
              assigned[task] = machine.name;
            } else {
              const std::string &foundName = found->second;
              if (foundName == machine.name) {
                err(machine, " has task ", task,
                    " assigned multiple times");
              } else {
                err(machine, " has task ", task,
                  " assigned which is already assigned to machine ",
                  foundName);
              }
              ++errors;
            }
          }
        });
  }
  return errors;
}

static inline void err(const Demand &demand, const string &msg) {
  cerr << "ERROR: Demand " << demand.name << msg << endl;
}

int CEImpl<Demand>::countErrors(const Demand &demand) {
  int errors = 0;
  if (!demand.destination.empty()) {
    edges[demand.destination] = demand.name;
    auto found = edges.find(demand.name);
    auto   end = edges.end();
    while (found != end) {
      const string &src = found->second;
      if (src == demand.destination) {
        err(demand, " participates in a loop");
        ++errors;
        break;
      }
      found = edges.find(src);
    }
  }
  if (demand.quantity <= 0) {
    err(demand, " has non-positive quantity");
    ++errors;
  }
  if (demand.interval <= 0) {
    err(demand, " has non-positive interval");
    ++errors;
  }
  if (demand.offset < 0) {
    err(demand, " has negative offset");
    ++errors;
  }
  return errors;
}

}

using Validate::CountErrors;
using Validate::CEImpl;

System *read(const string &filename) {
  Config cfg = YAML::LoadFile(filename).as<Config>();
  int errors = 0;
  if (cfg.logger.limit < 0) {
    cerr << "ERROR: Logger limit is negative" << endl;
    errors += 1;
  }
  errors +=
    accumulate(cfg.tasks.begin(), cfg.tasks.end(), 0,
      CountErrors<Task>(cfg.tasks)) +
    accumulate(cfg.machines.begin(), cfg.machines.end(), 0,
      CountErrors<Machine>(cfg.tasks)) +
    accumulate(cfg.demands.begin(), cfg.demands.end(), 0,
      CountErrors<Demand>(cfg.tasks));
  //TODO: more validation?
  if (errors > 0) {
    cerr << endl << "ERROR COUNT: " << errors << endl;
    return NULL;
  }
  //TODO: instantiation
  return NULL;
}

}
}

