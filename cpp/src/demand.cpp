#include <map>
#include "demand.hpp"
#include "task.hpp"
#include "event_queue.hpp"

namespace SK3 {

using namespace std;

namespace {

template<class D> shared_ptr<Demand> factory(const Config::Demand &cfg,
    shared_ptr<EventQueue> eventQ, TaskMap &tasks) {
  shared_ptr<Task> task = tasks.at(cfg.name);
  return make_shared<D>(eventQ, task,
      Quantity(cfg.quantity),
      Time(cfg.interval),
      Time(cfg.offset));
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
  offset(_offset),
  counter(0),
  intervalFunc(bind(&Demand::onInterval, ref(*this))) { }

void Demand::init_sim() {
  eventQ->add_event(interval + offset, intervalFunc);
}

void Demand::sendOrder() {
  task.lock()->take_from_buffer(quantity,
      make_shared<Order>(shared_from_this(), counter++));
}

void Demand::onInterval() {
  eventQ->add_event(interval, intervalFunc);
  sendOrder();
}

Demand::Order::DemandOrder(shared_ptr<Demand> _demand, long _serial):
  eventQ(_demand->eventQ),
  startTime(_demand->eventQ->now()),
  demandName(_demand->task.lock()->name()),
  serial(to_string(_serial)) { }

void Demand::Order::taking(weak_ptr<Task> task, Quantity quantity) {
  products[task] -= quantity;
}

typedef EventQueue::LogRecord LogRecord;
typedef LogRecord::Type LogType;

pair<bool, Quantity> Demand::Order::fulfilling(weak_ptr<Task> task,
    Quantity quantity) {
  long value = products[task] + quantity;
  bool fulfilled = (value >= 0);
  if (fulfilled) {
    products.erase(task);
    if (products.empty()) {
      Time duration = eventQ->now() - startTime;
      eventQ->log(
          LogRecord(LogType::LOG_RIPPLE_END, duration, serial),
          *this);
    }
  } else {
    products[task] = value;
    value = 0;
  }
  return make_pair(fulfilled, value);
}

const string &Demand::Order::name() const { return demandName; }
const Quantity Demand::Order::buffer() const { return 0; }

} // namespace SK3

