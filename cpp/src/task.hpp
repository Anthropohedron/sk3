#ifndef TASK_HPP
#define TASK_HPP

#include <string>
#include "sk3.hpp"
#include "logger.hpp"
#include "config/representation.hpp"

namespace SK3 {

class Machine;
class Demand;

class Task : public LogReporter, SimulationComponent {

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

  friend Machine;
  Machine *machine;

  virtual bool should_enqueue();

};

}

#endif //TASK_HPP

