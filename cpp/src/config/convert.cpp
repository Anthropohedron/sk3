#include <iostream>
#include <yaml-cpp/yaml.h>
#include "../sk3.hpp"
#include "representation.hpp"
#include "convert.hpp"

using std::map;
using std::vector;
using std::pair;
using std::string;
using SK3::StringMap;

template<class T>
static inline void setNames(StringMap<T> &dict) {
  for_each(dict.begin(), dict.end(),
      [](pair<const string, T> &val) {
        val.second.name = val.first;
      });
}

namespace YAML {

static inline bool isScalar(const Node &node) {
  return node.IsDefined() && node.IsScalar();
}

static inline bool isMap(const Node &node) {
  return node.IsDefined() && node.IsMap();
}

bool convert<SK3C::Logger>::decode(const Node &node, SK3C::Logger &rhs) {
  Node file  = node["file"];
  Node dir   = node["dir"];
  Node limit = node["limit"];
  rhs.file  =  isScalar(file) ?  file.as<string>() : "";
  rhs.dir   =   isScalar(dir) ?   dir.as<string>() : "";
  rhs.limit = isScalar(limit) ? limit.as<double>() :  0;
  return true;
}

bool convert<SK3C::Variants>::decode(const Node &node, SK3C::Variants &rhs) {
  if (!isMap(node)) {
    rhs.task    = "Task";
    rhs.machine = "Machine";
    rhs.demand  = "Demand";
  } else {
    Node task    = node["task"];
    Node machine = node["machine"];
    Node demand  = node["demand"];
    rhs.task    =    isScalar(task) ?    task.as<string>() : "Task";
    rhs.machine = isScalar(machine) ? machine.as<string>() : "Machine";
    rhs.demand  =  isScalar(demand) ?  demand.as<string>() : "Demand";
  }
  return true;
}

bool convert<SK3C::Task>::decode(const Node &node, SK3C::Task &rhs) {
  if (!isMap(node)) return false;
  Node batch_size = node["batch_size"];
  Node batch_time = node["batch_time"];
  Node unit_time  = node["unit_time"];
  Node deficit    = node["deficit"];
  Node debit      = node["debit"];
  if (!(isScalar(batch_size) &&
        (isScalar(batch_time) || isScalar(unit_time)))) {
    return false;
  }
  rhs.batch_size = batch_size.as<double>();
  rhs.batch_time = isScalar(batch_time) ? batch_time.as<double>() :
    unit_time.as<double>() * rhs.batch_size;
  rhs.deficit = isScalar(deficit) ? deficit.as<double>() :
    isScalar(debit) ? debit.as<double>() : 0;
  return true;
}

bool convert<SK3C::Machine>::decode(const Node &node, SK3C::Machine &rhs) {
  if (!node.IsSequence()) return false;
  rhs.tasks = node.as<vector<string>>();
  return true;
}

static inline bool isDemand(const Node &destination) {
  return !destination.IsDefined() ||
    (isScalar(destination) && destination.as<string>() == "external");
}

bool convert<SK3C::Demand>::decode(const Node &node, SK3C::Demand &rhs) {
  if (!isMap(node)) return false;
  Node    supplier = node["supplier"];
  Node destination = node["destination"];
  Node quantity    = node["quantity"];
  Node interval    = node["interval"];
  Node offset      = node["offset"];
  if (!isScalar(supplier) ||
      !isScalar(quantity) ||
      !isScalar(interval) ||
      !isDemand(destination)) {
    return false;
  }
  rhs.name = supplier.as<string>();
  rhs.interval = interval.as<double>();
  rhs.offset   = isScalar(offset) ? offset.as<double>() : 0;
  rhs.quantity = quantity.as<double>();
  return true;
}

static inline bool isPrereq(const Node &destination) {
  return isScalar(destination) && destination.as<string>() != "external";
}

bool convert<SK3C::Prereq>::decode(const Node &node, SK3C::Prereq &rhs) {
  if (!isMap(node)) return false;
  Node supplier    = node["supplier"];
  Node destination = node["destination"];
  Node quantity    = node["quantity"];
  Node interval    = node["interval"];
  if (!isScalar(supplier) ||
      !isScalar(quantity) ||
      interval.IsDefined() ||
      !isPrereq(destination)) {
    return false;
  }
  rhs.supplyingTask = supplier.as<string>();
  rhs.receivingTask = destination.as<string>();
  rhs.quantity = quantity.as<double>();
  rhs.name = rhs.supplyingTask + " -> " + rhs.receivingTask;
  return true;
}

static inline void splitDemands(const Node &nodes,
    StringMap<SK3C::Demand> &demands,
    vector<SK3C::Prereq> &prereqs) {
  for_each(nodes.begin(), nodes.end(),
      [&demands,&prereqs](const Node &node) {
        try {
          SK3C::Prereq prereq = node.as<SK3C::Prereq>();
          prereqs.push_back(prereq);
        } catch (const BadConversion &) {
          SK3C::Demand demand = node.as<SK3C::Demand>();
          if (demands.find(demand.name) != demands.end()) {
            throw TypedBadConversion<SK3C::Demand>();
          }
          demands[demand.name] = demand;
        }
      });
}

bool convert<SK3C::Config>::decode(const Node &node, SK3C::Config &rhs) {
  if (!isMap(node)) return false;
  rhs.logger = node["logger"].as<SK3C::Logger>();
  if (isMap(node["variants"])) {
    rhs.variants = node["variants"].as<SK3C::Variants>();
  } else {
    rhs.variants.task    = "Task";
    rhs.variants.machine = "Machine";
    rhs.variants.demand  = "Demand";
  }
  rhs.tasks = node["tasks"].as<StringMap<SK3C::Task>>();
  setNames(rhs.tasks);
  rhs.machines = node["machines"].as<StringMap<SK3C::Machine>>();
  setNames(rhs.machines);
  splitDemands(node["demands"], rhs.demands, rhs.prereqs);
  return true;
}

} // namespace YAML

