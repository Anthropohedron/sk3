#ifndef MACHINE_HPP
#define MACHINE_HPP

#include <map>
#include <queue>
#include "sk3.hpp"
#include "logger.hpp"
#include "config/representation.hpp"

namespace SK3 {

class Machine : public LogReporter, SimulationComponent {

  public:

  static machine_factory_t factoryFor(const std::string &variant);

  Machine(shared_ptr<EventQueue> _eventQ, const std::string &_name,
      const std::map<shared_ptr<Task>, long> &_tasks);

  // config
  void add_task(Task *task);

  // operation
  void enqueue(Task *task);

  virtual void init_sim();

  virtual const std::string name() const;
  virtual const Quantity buffer() const;

  private:

  shared_ptr<EventQueue> eventQ;
  const std::string machineName;
  std::map<shared_ptr<Task>, long> tasks;
  std::queue<shared_ptr<Task>> runQ;
  shared_ptr<Task> doing;

};

}

#endif //MACHINE_HPP

