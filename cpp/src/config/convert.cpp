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

bool convert<SK3C::Logger>::decode(const Node &node, SK3C::Logger &rhs) {
  Node file  = node["file"];
  Node dir   = node["dir"];
  Node limit = node["limit"];
  rhs.file  =  file.IsScalar() ?  file.as<string>() : "";
  rhs.dir   =   dir.IsScalar() ?   dir.as<string>() : "";
  rhs.limit = limit.IsScalar() ? limit.as<double>()   :  0;
  return true;
}

bool convert<SK3C::Variants>::decode(const Node &node, SK3C::Variants &rhs) {
  if (!node.IsMap()) {
    rhs.task    = "Task";
    rhs.machine = "Machine";
    rhs.demand  = "Demand";
  } else {
    Node task    = node["task"];
    Node machine = node["machine"];
    Node demand  = node["demand"];
    rhs.task    =    task.IsScalar() ?    task.as<string>() : "Task";
    rhs.machine = machine.IsScalar() ? machine.as<string>() : "Machine";
    rhs.demand  =  demand.IsScalar() ?  demand.as<string>() : "Demand";
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
  rhs.tasks = node.as<vector<string>>();
  return true;
}

static inline bool isDemand(const Node &destination) {
  return !destination.IsDefined() ||
    (destination.IsScalar() && destination.as<string>() == "external");
}

bool convert<SK3C::Demand>::decode(const Node &node, SK3C::Demand &rhs) {
  if (!node.IsMap()) return false;
  Node    supplier = node["supplier"];
  Node destination = node["destination"];
  Node quantity    = node["quantity"];
  Node interval    = node["interval"];
  Node offset      = node["offset"];
  if (!supplier.IsScalar() ||
      !quantity.IsScalar() ||
      !interval.IsScalar() ||
      !isDemand(destination)) {
    return false;
  }
  rhs.name = supplier.as<string>();
  rhs.interval = interval.as<double>();
  rhs.offset   = offset.IsScalar() ? offset.as<double>() : 0;
  rhs.quantity = quantity.as<double>();
  return true;
}

static inline bool isPrereq(const Node &destination) {
  return destination.IsScalar() && destination.as<string>() != "external";
}

bool convert<SK3C::Prereq>::decode(const Node &node, SK3C::Prereq &rhs) {
  if (!node.IsMap()) return false;
  Node supplier    = node["supplier"];
  Node destination = node["destination"];
  Node quantity    = node["quantity"];
  Node interval    = node["interval"];
  if (!supplier.IsScalar() ||
      !quantity.IsScalar() ||
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
  if (!node.IsMap()) return false;
  rhs.logger = node["logger"].as<SK3C::Logger>();
  rhs.variants = node["variants"].as<SK3C::Variants>();
  rhs.tasks = node["tasks"].as<StringMap<SK3C::Task>>();
  setNames(rhs.tasks);
  rhs.machines = node["machines"].as<StringMap<SK3C::Machine>>();
  setNames(rhs.machines);
  splitDemands(node["demands"], rhs.demands, rhs.prereqs);
  return true;
}

} // namespace YAML

