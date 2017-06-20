#ifndef MACHINE_HPP
#define MACHINE_HPP

#include <map>
#include <queue>
#include "sk3.hpp"
#include "logger.hpp"

namespace SK3 {

class Machine : public LogReporter, SimulationComponent {

  public:

  Machine(EventQueue &_eventQ, const std::string &_name);

  // config
  void add_task(Task *task);

  // operation
  void enqueue(Task *task);

  private:

  EventQueue &eventQ;
  const std::string machineName;
  std::map<Task *, long> tasks;
  std::queue<Task *> runQ;
  Task *doing;

};

}

#endif //MACHINE_HPP

