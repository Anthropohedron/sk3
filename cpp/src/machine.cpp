#include <algorithm>
#include <numeric>
#include <sstream>
#include "machine.hpp"
#include "task.hpp"
#include "event_queue.hpp"

namespace SK3 {

using namespace std;

namespace {

template<class M> shared_ptr<Machine> factory(const Config::Machine &cfg,
    shared_ptr<EventQueue> eventQ, TaskMap &tasks) {
  Machine::TaskCounts taskCounts;
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
    const TaskCounts &_tasks):
  eventQ(_eventQ),
  machineName(_name),
  eventFunc(bind(&Machine::finishTask, ref(*this))),
  tasks(_tasks) { }

void Machine::startNextTask() {
  if (!idle || runQ.empty()) return;
  idle = false;
  shared_ptr<Task> doing = runQ.front().lock();
  doing->startBatch();
  ++tasks[doing];
  eventQ->add_event(doing->batch_time, eventFunc);
}

typedef EventQueue::LogRecord LogRecord;
typedef LogRecord::Type LogType;

void Machine::finishTask() {
  if (idle) return;
  shared_ptr<Task> doing = runQ.front().lock();
  runQ.pop_front();
  doing->finishBatch();
  idle = true;
  startNextTask();
  if (idle) {
    idleStart = eventQ->now();
    ostringstream stats;
    bool first = true;
    Time duration = accumulate(tasks.begin(), tasks.end(), 0,
        [&stats,&first](Time sum, pair<const shared_ptr<Task>, long> &val) {
          const long count = val.second;
          shared_ptr<Task> task = val.first;
          if (first) {
            first = false;
          } else {
            stats << " ";
          }
          stats << task->name() << ':' << count
            << ',' << task->resetLowWaterMark();
          val.second = 0;
          return sum + task->batch_time * count;
        });
    eventQ->log(
      LogRecord(LogType::LOG_ACTIVE_END, duration, stats.str()),
      *this);
  }
}

void Machine::enqueue(shared_ptr<Task> task) {
  if (tasks.find(task) == tasks.end()) {
    // can't queue a task not assigned to this machine
    ostringstream errMsg;
    errMsg << "task '" << task->name()
      << "' is not assigned to machine '" << machineName
      << "'";
    throw halt_simulation(eventQ->now(), errMsg.str());
  }
  if (find(runQ.begin(), runQ.end(), weak_ptr<Task>(task)) != runQ.end()) {
    // already queued
    return;
  }
  runQ.push_back(task);
  bool wasIdle = idle;
  startNextTask();
  if (wasIdle && !idle) {
    Time duration = eventQ->now() - idleStart;
    if (duration <= 0) return;
    eventQ->log(
      LogRecord(LogType::LOG_IDLE_END, duration),
      *this);
  }
}

void Machine::init_sim() {
  idle = true;
  runQ.clear();
  idleStart = eventQ->now();
}

const string &Machine::name() const { return machineName; }
const Quantity Machine::buffer() const { return 0; }

}

