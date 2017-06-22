#ifndef MACHINE_HPP
#define MACHINE_HPP

#include <map>
#include <queue>
#include "sk3.hpp"
#include "logger.hpp"
#include "config/representation.hpp"

namespace SK3 {

namespace Instantiate {
class Factory;
}

class Machine :
public LogReporter,
public SimulationComponent {

  public:

  typedef Config::Machine config_type;

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

  friend Instantiate::Factory;
  std::map<shared_ptr<Task>, long> tasks;
  std::queue<weak_ptr<Task>> runQ;
  weak_ptr<Task> doing;

};

}

#endif //MACHINE_HPP

