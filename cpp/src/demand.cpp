#include <map>
#include "demand.hpp"

namespace SK3 {

using namespace std;

namespace {

template<class D> shared_ptr<Demand> factory(const Config::Demand &cfg,
    shared_ptr<EventQueue> eventQ, TaskMap &tasks) {
  shared_ptr<Task> task = tasks.at(cfg.name);
  return make_shared<D>(eventQ, task,
      to_internal_quantity(cfg.quantity),
      to_internal_time(cfg.interval),
      to_internal_time(cfg.offset));
}

static map<string, demand_factory_t> factories {
  { "Demand", factory<Demand> }
};

} // anon namespace

demand_factory_t Demand::factoryFor(const string &variant) {
  return factories.at(variant);
}

Demand::Demand(shared_ptr<EventQueue> _eventQ, weak_ptr<Task> _task,
    Quantity _quantity, Time _interval, Time _offset):
  eventQ(_eventQ),
  task(_task),
  quantity(_quantity),
  interval(_interval),
  offset(_offset) { }

void Demand::init_sim() {
  //TODO
}

} // namespace SK3

