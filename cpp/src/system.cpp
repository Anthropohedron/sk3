#include <iostream>
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
    const StringPtrMap<Task> &_tasks,
    const StringPtrMap<Machine> &_machines,
    const StringPtrMap<Demand> &_demands
    ):
  eventQ(_eventQ),
  tasks(_tasks),
  machines(_machines),
  demands(_demands) { }

void System::runUntil(Time endTime) {
  init_sim();
  continueUntil(endTime);
}

void System::continueUntil(Time endTime) {
  try {
    while (eventQ->runOneBefore(endTime));
    cerr << "Simulation completed normally at ";
    time_format(cerr, eventQ->now());
    cerr << endl;
  } catch (const halt_simulation halt) {
    cerr << "Halting simulation at ";
    time_format(cerr, halt.end);
    cerr << ": " << halt.msg << endl;
  }
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

