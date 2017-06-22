#include "task.hpp"

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
  eventQ(_eventQ),
  taskName(_name),
  batch_time(_batch_time),
  batch_size(_batch_size),
  taskBuffer(init_buffer) { }

void Task::init_sim() {
  //TODO
}

const string &Task::name() const { return taskName; }
const Quantity Task::buffer() const { return taskBuffer; }

void Task::take_from_buffer(const Quantity quantity,
    Demand *demand) {
  //TODO
}

bool Task::should_enqueue() {
  //TODO
  return false;
}

}

