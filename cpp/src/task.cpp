#include <algorithm>
#include "task.hpp"
#include "machine.hpp"
#include "demand.hpp"
#include "event_queue.hpp"

namespace SK3 {

using namespace std;

namespace {

template<class T> shared_ptr<Task> factory(const Config::Task &cfg,
    shared_ptr<EventQueue> eventQ) {
  return make_shared<T>(eventQ, cfg.name,
      to_internal_time(cfg.batch_time),
      to_internal_quantity(cfg.batch_size),
      to_internal_quantity(cfg.deficit));
}

static map<string, task_factory_t> factories {
  { "Task", factory<Task> }
};

} // anon namespace


task_factory_t Task::factoryFor(const string &variant) {
  return factories.at(variant);
}

Task::Task(shared_ptr<EventQueue> _eventQ, const string &_name,
    const Time _batch_time, const Quantity _batch_size,
    const Quantity init_buffer):
  taskName(_name),
  batch_time(_batch_time),
  batch_size(_batch_size),
  eventQ(_eventQ),
  taskBuffer(init_buffer),
  low_water_mark(init_buffer) { }

void Task::init_sim() {
  if (taskBuffer < 0) {
    startTime = eventQ->now();
    machine.lock()->enqueue(shared_from_this());
  }
  resetLowWaterMark();
}

const string &Task::name() const { return taskName; }
const Quantity Task::buffer() const { return taskBuffer; }

void Task::take_from_buffer(const Quantity quantity,
    shared_ptr<DemandOrder> order) {
  if ((taskBuffer >= 0) && (taskBuffer < quantity)) {
    startTime = eventQ->now();
  }
  taskBuffer -= quantity;
  if (taskBuffer < low_water_mark) {
    low_water_mark = taskBuffer;
  }
  if (should_enqueue()) {
    machine.lock()->enqueue(shared_from_this());
  }
  if (taskBuffer >= 0) return;
  //FIXME: max seems wrong; I think it should be min?
  order->taking(shared_from_this(), max(quantity, -taskBuffer));
  if (find(orders.begin(), orders.end(), order) == orders.end()) {
    orders.push_back(order);
  }
}

void Task::startBatch() {
  shared_ptr<DemandOrder> serving = orders.front();
  for_each(suppliers.begin(), suppliers.end(),
      [serving](Supplier &supplier) {
        shared_ptr<Task> task = supplier.task.lock();
        task->take_from_buffer(supplier.quantity, serving);
      });
}

typedef EventQueue::LogRecord LogRecord;
typedef LogRecord::Type LogType;

void Task::finishBatch() {
  Quantity produced = batch_size;
  taskBuffer += produced;
  while ((produced > 0) && !orders.empty()) {
    pair<bool, Quantity> fulfilled =
      orders.front()->fulfilling(shared_from_this(), produced);
    if (fulfilled.first) {
      orders.pop_front();
    }
    produced = fulfilled.second;
  }
  if (taskBuffer < 0) {
    machine.lock()->enqueue(shared_from_this());
  } else {
    Time duration = eventQ->now() - startTime;
    eventQ->log(
        LogRecord(LogType::LOG_BUFFER_FULL, duration),
        *this);
  }
}

Quantity Task::resetLowWaterMark() {
  Quantity low = low_water_mark;
  low_water_mark = taskBuffer;
  return low;
}

bool Task::should_enqueue() {
  return taskBuffer < 0;
  return false;
}

}

