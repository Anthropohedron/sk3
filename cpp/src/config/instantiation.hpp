#ifndef CONFIG__INSTANTIATION_HPP
#define CONFIG__INSTANTIATION_HPP
#include "representation.hpp"

namespace SK3 {

namespace Instantiate {

class Factory {

  public:

  Factory(const Config::Variants &variants);

  template<class Instance,
    class ConfigClass = typename Instance::config_type>
  shared_ptr<Instance> create(const ConfigClass &cfg);

  template<class Instance,
    class ConfigClass = typename Instance::config_type>
  void create(const StringMap<ConfigClass> &configs,
      StringPtrMap<Instance> &out);

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

