#include "instantiation.hpp"
#include "../event_queue.hpp"
#include "../logger.hpp"
#include "../task.hpp"
#include "../machine.hpp"
#include "../demand.hpp"

namespace SK3 {
namespace Instantiate {

using std::shared_ptr;

Factory::Factory(const Config::Variants &variants):
  eventQ(new EventQueue()),
     taskFactory(   Task::factoryFor(variants.task)),
  machineFactory(Machine::factoryFor(variants.machine)),
   demandFactory( Demand::factoryFor(variants.demand))
{
  //TODO
}

shared_ptr<Logger> Factory::createLogger(const Config::Logger &cfg) {
  //TODO
  return NULL;
}

} // namespace SK3::Instantiate
} // namespace SK3

