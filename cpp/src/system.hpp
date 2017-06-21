#ifndef SYSTEM_H
#define SYSTEM_H

#include "sk3.hpp"

namespace SK3 {

class EventQueue;
class Logger;
class Task;
class Machine;
class Demand;

namespace Config {
struct Config;
}

class System {

  public:

  typedef Config::Config config_type;

  System(
    shared_ptr<EventQueue> _eventQ,
    shared_ptr<Logger> _logger,
    const StringPtrMap<Task> &_tasks,
    const StringPtrMap<Machine> &_machines,
    const StringPtrMap<Demand> &_demands
    );

  void runUntil(Time endTime);

  void continueUntil(Time endTime);
  void init_sim();

  private:

  shared_ptr<EventQueue> eventQ;
  shared_ptr<Logger> logger;
  StringPtrMap<Task> tasks;
  StringPtrMap<Machine> machines;
  StringPtrMap<Demand> demands;

};

}

#endif //SYSTEM_H

