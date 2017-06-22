#ifndef TASK_HPP
#define TASK_HPP

#include <string>
#include <vector>
#include "sk3.hpp"
#include "logger.hpp"
#include "config/representation.hpp"

namespace SK3 {

class Task;
class Machine;
class Demand;

namespace Instantiate {
class Factory;
}

struct Supplier {
  inline Supplier(weak_ptr<Task> _task, Quantity _quantity):
    task(_task), quantity(_quantity) { }
  weak_ptr<Task> task;
  Quantity quantity;
};

class Task :
public LogReporter,
public SimulationComponent {

  public:

  typedef Config::Task config_type;

  static task_factory_t factoryFor(const std::string &variant);

  Task(shared_ptr<EventQueue> _eventQ, const std::string &_name,
      const Time _batch_time, const Quantity _batch_size,
      const Quantity init_buffer = 0);

  virtual void init_sim();

  virtual const std::string name() const;
  virtual const Quantity buffer() const;

  virtual void take_from_buffer(const Quantity quantity,
      Demand *demand = NULL);

  protected:

  shared_ptr<EventQueue> eventQ;
  const std::string taskName;
  const Time batch_time;
  const Quantity batch_size;
  Quantity taskBuffer;
  Quantity low_water_mark;

  friend Instantiate::Factory;
  weak_ptr<Machine> machine;
  std::vector<Supplier> suppliers;

  virtual bool should_enqueue();

};

}

#endif //TASK_HPP

