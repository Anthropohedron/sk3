#include <algorithm>
#include "system.hpp"
#include "event_queue.hpp"
#include "task.hpp"
#include "machine.hpp"
#include "demand.hpp"

namespace SK3 {

using namespace std;

System::System(
    shared_ptr<EventQueue> _eventQ,
    shared_ptr<Logger> _logger,
    const StringPtrMap<Task> &_tasks,
    const StringPtrMap<Machine> &_machines,
    const StringPtrMap<Demand> &_demands
    ):
  eventQ(_eventQ),
  logger(_logger),
  tasks(_tasks),
  machines(_machines),
  demands(_demands) { }

void System::runUntil(Time endTime) {
  init_sim();
  continueUntil(endTime);
}

void System::continueUntil(Time endTime) {
  while (eventQ->runOneBefore(endTime));
}

namespace {

template<class Instance>
inline void initInstance(StringPtrPair<Instance> &val) {
  val.second->init_sim();
}

}

void System::init_sim() {
  eventQ->init_sim();
  for_each(machines.begin(), machines.end(), initInstance<Machine>);
  for_each(tasks.begin(), tasks.end(), initInstance<Task>);
  for_each(demands.begin(), demands.end(), initInstance<Demand>);
}

} // namespace SK3

