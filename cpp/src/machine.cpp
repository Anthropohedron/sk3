#include <algorithm>
#include "machine.hpp"

namespace SK3 {

using namespace std;

namespace {

template<class M> shared_ptr<Machine> factory(const Config::Machine &cfg,
    shared_ptr<EventQueue> eventQ, TaskMap &tasks) {
  map<shared_ptr<Task>, long> taskCounts;
  for_each(cfg.tasks.begin(), cfg.tasks.end(),
      [&tasks,&taskCounts](const string &taskName) {
        taskCounts[tasks.at(taskName)] = 0L;
      });
  return make_shared<M>(eventQ, cfg.name, taskCounts);
}

static map<string, machine_factory_t> factories {
  { "Machine", factory<Machine> }
};

} // anon namespace


machine_factory_t Machine::factoryFor(const string &variant) {
  return factories.at(variant);
}

Machine::Machine(shared_ptr<EventQueue> _eventQ, const string &_name,
    const map<shared_ptr<Task>, long> &_tasks):
  eventQ(_eventQ),
  machineName(_name),
  tasks(_tasks) { }

void Machine::init_sim() {
  //TODO
}

const string Machine::name() const {
  return machineName;
}
const Quantity Machine::buffer() const {
  return 0;
}

}

