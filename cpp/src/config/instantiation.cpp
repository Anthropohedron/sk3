#include "instantiation.hpp"
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

shared_ptr<Task> Factory::createTask(const Config::Task &cfg) {
  string name = cfg.name;
  shared_ptr<Task> task = taskFactory(cfg, eventQ);
  tasks[name] = task;
  return task;
}

shared_ptr<Logger> Factory::createLogger(const Config::Logger &cfg) {
  double limit = (cfg.limit <= 0) ? 1000000 : cfg.limit;
  if (cfg.dir.empty()) {
    return make_shared<SimpleLogger>(to_internal_time(limit), cfg.file);
  } else {
    return make_shared<SplitLogger>(limit, cfg.dir);
  }
}

} // namespace SK3::Instantiate
} // namespace SK3

