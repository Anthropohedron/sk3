#ifndef CONFIG__INSTANTIATION_HPP
#define CONFIG__INSTANTIATION_HPP
#include "representation.hpp"

namespace SK3 {

class Logger;

namespace Instantiate {

class Factory {

  public:

  Factory(const Config::Variants &variants);

  shared_ptr<Logger> createLogger(const Config::Logger &cfg);

  shared_ptr<Task> createTask(const Config::Task &cfg);

  inline shared_ptr<Machine> createMachine(const Config::Machine &cfg) {
    return machineFactory(cfg, eventQ, tasks);
  }

  inline shared_ptr<Demand> createDemand(const Config::Demand &cfg) {
    return demandFactory(cfg, eventQ, tasks);
  }

  void createTasks(const StringMap<Config::Task> &configs);
  void createMachines(const StringMap<Config::Machine> &configs,
      StringPtrMap<Machine> &out);
  void createDemands(const StringMap<Config::Demand> &configs,
      StringPtrMap<Demand> &out);

  inline const TaskMap &getTasks() { return tasks; }

  inline shared_ptr<EventQueue> getQueue() { return eventQ; }

  private:

  TaskMap tasks;
  shared_ptr<EventQueue> eventQ;
  const    task_factory_t taskFactory;
  const machine_factory_t machineFactory;
  const  demand_factory_t demandFactory;

};

} // namespace SK3::Instantiate
} // namespace SK3

#endif //CONFIG__INSTANTIATION_HPP

