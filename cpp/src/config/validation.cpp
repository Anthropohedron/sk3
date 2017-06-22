#include <iostream>
#include <numeric>
#include <deque>
#include "validation.hpp"

namespace SK3 {
namespace Config {

const std::string   Logger::NAME = "Logger";
const std::string Variants::NAME = "Variants";
const std::string     Task::NAME = "Task";
const std::string  Machine::NAME = "Machine";
const std::string   Demand::NAME = "Demand";
const std::string   Prereq::NAME = "Prereq";
const std::string   Config::NAME = "Config";

namespace Validate {

using namespace std;

template <typename C>
static inline void err(const C &cfg, const string &msg,
    const string &msg2 = "",
    const string &msg3 = "",
    const string &msg4 = "") {
  cerr << "ERROR: " << C::NAME << " " << cfg.name
    << msg << msg2 << msg3 << msg4 << endl;
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

int CEImpl<Demand>::countErrors(const Demand &demand) {
  int errors = 0;
  if (knownTasks.find(demand.name) == knownTasks.end()) {
    err(demand, " is for an unknown task");
    ++errors;
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

int CEImpl<Prereq>::countErrors(const Prereq &prereq) {
  int errors = 0;
  set<string> &suppliers = edges[prereq.receivingTask];
  if (!suppliers.insert(prereq.supplyingTask).second) {
    err(prereq, " is duplicated");
    ++errors;
  }
  set<string> checked;
  deque<string> toCheck(suppliers.begin(), suppliers.end());
  while (!toCheck.empty()) {
    const string name = toCheck.front();
    if (name == prereq.receivingTask) {
      err(prereq, " participates in a loop");
      ++errors;
      break;
    }
    toCheck.pop_front();
    if (!checked.insert(name).second) continue;
    StringMap<set<string>>::iterator found = edges.find(name);
    if (found == edges.end()) continue;
    toCheck.insert(toCheck.end(),
        found->second.begin(), found->second.end());
  }
  if (knownTasks.find(prereq.supplyingTask) == knownTasks.end()) {
    err(prereq, " is for an unknown source task");
    ++errors;
  }
  if (knownTasks.find(prereq.receivingTask) == knownTasks.end()) {
    err(prereq, " has an unknown destination task ", prereq.receivingTask);
    ++errors;
  }
  if (prereq.quantity <= 0) {
    err(prereq, " has non-positive quantity");
    ++errors;
  }
  return errors;
}

static inline int countUnusedTasks(
    const vector<Prereq> &prereqs,
    const StringMap<Demand> &demands,
    const StringMap<Task> &tasks) {
  StringMap<Task> unused = tasks;
  for_each(prereqs.begin(), prereqs.end(),
      [&unused](const Prereq &prereq) {
        unused.erase(prereq.supplyingTask);
      });
  for_each(demands.begin(), demands.end(),
      [&unused](const pair<const string, Demand> &val) {
        unused.erase(val.first);
      });
  for_each(unused.begin(), unused.end(),
      [](const pair<const string, Task> &val) {
        err(val.second, " is not used by any task or external demand");
      });
  return unused.size();
}

int countErrors(const Config &cfg) {
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
      CountErrors<Demand>(cfg.tasks)) +
    accumulate(cfg.prereqs.begin(), cfg.prereqs.end(), 0,
      CountErrors<Prereq>(cfg.tasks)) +
    countUnusedTasks(cfg.prereqs, cfg.demands, cfg.tasks);
  //TODO: more validation?
  return errors;
}

} // namespace SK3::Config::Validate
} // namespace SK3::Config
} // namespace SK3

