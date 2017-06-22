#ifndef MACHINE_HPP
#define MACHINE_HPP

#include <map>
#include <deque>
#include "sk3.hpp"
#include "logger.hpp"
#include "task.hpp"
#include "config/representation.hpp"

namespace SK3 {

namespace Instantiate {
class Factory;
}

class Machine :
public LogReporter,
public SimulationComponent {

  public:

  typedef std::map<shared_ptr<Task>, long, Task::CompareByName> TaskCounts;
  typedef Config::Machine config_type;

  static machine_factory_t factoryFor(const std::string &variant);

  Machine(shared_ptr<EventQueue> _eventQ, const std::string &_name,
      const TaskCounts &_tasks);

  void enqueue(shared_ptr<Task> task);

  virtual void init_sim();

  virtual const std::string &name() const;
  virtual const Quantity buffer() const;

  private:

  shared_ptr<EventQueue> eventQ;
  const std::string machineName;
  Func eventFunc;

  friend Instantiate::Factory;
  TaskCounts tasks;
  std::deque<weak_ptr<Task>> runQ;

  bool idle;
  Time idleStart;

  void startNextTask();
  void finishTask();

};

}

#endif //MACHINE_HPP

