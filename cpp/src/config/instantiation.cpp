#include <algorithm>
#include "instantiation.hpp"
#include "../system.hpp"
#include "../event_queue.hpp"
#include "../logger.hpp"
#include "../task.hpp"
#include "../machine.hpp"
#include "../demand.hpp"

namespace SK3 {
namespace Instantiate {

using namespace std;

Factory::Factory(const Config::Variants &variants):
  eventQ(new EventQueue()),
     taskFactory(   Task::factoryFor(variants.task)),
  machineFactory(Machine::factoryFor(variants.machine)),
   demandFactory( Demand::factoryFor(variants.demand)) { }


template<>
shared_ptr<Task> Factory::create(const Config::Task &cfg) {
  string name = cfg.name;
  shared_ptr<Task> task = taskFactory(cfg, eventQ);
  tasks[name] = task;
  return task;
}

template<>
shared_ptr<Machine> Factory::create(const Config::Machine &cfg) {
  shared_ptr<Machine> machine = machineFactory(cfg, eventQ, tasks);
  for_each(machine->tasks.begin(), machine->tasks.end(),
      [machine](pair<const shared_ptr<Task>, long> &val) {
        val.first->machine = machine;
      });
  return machine;
}

template<>
shared_ptr<Demand> Factory::create(const Config::Demand &cfg) {
  return demandFactory(cfg, eventQ, tasks);
}

template<>
shared_ptr<Logger> Factory::create(const Config::Logger &cfg) {
  double limit = (cfg.limit <= 0) ? 1000000 : cfg.limit;
  if (cfg.dir.empty()) {
    return make_shared<SimpleLogger>(to_internal_time(limit), cfg.file);
  } else {
    return make_shared<SplitLogger>(limit, cfg.dir);
  }
}

template<class Instance, class ConfigClass>
void Factory::create(const StringMap<ConfigClass> &configs,
    StringPtrMap<Instance> &out) {
  for_each(configs.begin(), configs.end(),
      [&out,this](const pair<string, ConfigClass> &val) {
        out[val.first] = this->create<Instance>(val.second);
      });
}

template<>
shared_ptr<System> Factory::create(const Config::Config &cfg) {
  shared_ptr<Logger> logger = create<Logger>(cfg.logger);
  eventQ->set_logger(logger);
  StringPtrMap<Task> tasks;
  StringPtrMap<Machine> machines;
  StringPtrMap<Demand> demands;
  create<Task>(cfg.tasks, tasks);
  create<Machine>(cfg.machines, machines);
  create<Demand>(cfg.demands, demands);
  return make_shared<System>(eventQ, logger, tasks, machines, demands);
}

shared_ptr<System> Factory::createSystem(const Config::Config &cfg) {
  Factory factory(cfg.variants);
  return factory.create<System>(cfg);
}

} // namespace SK3::Instantiate
} // namespace SK3

