#ifndef TASK_HPP
#define TASK_HPP

#include <string>
#include <vector>
#include <deque>
#include "sk3.hpp"
#include "logger.hpp"
#include "config/representation.hpp"

namespace SK3 {

class Task;
class Machine;
class DemandOrder;

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
public std::enable_shared_from_this<Task>,
public LogReporter,
public SimulationComponent {

  public:

  struct CompareByName {
    inline bool operator()(const Task &lhs,
        const Task &rhs) const {
      return lhs.taskName < rhs.taskName;
    }
    inline bool operator()(shared_ptr<Task> lhs,
        shared_ptr<Task> rhs) const {
      return lhs->taskName < rhs->taskName;
    }
  };

  typedef Config::Task config_type;

  static task_factory_t factoryFor(const std::string &variant);

  Task(shared_ptr<EventQueue> _eventQ, const std::string &_name,
      const Time _batch_time, const Quantity _batch_size,
      const Quantity init_buffer = Quantity());

  virtual void init_sim();

  virtual const std::string &name() const;
  virtual const Quantity buffer() const;

  virtual void take_from_buffer(const Quantity quantity);
  virtual void take_from_buffer(const Quantity quantity,
      shared_ptr<DemandOrder> order);

  void startBatch();
  void finishBatch();
  Quantity resetLowWaterMark();

  const std::string taskName;
  const Time batch_time;

  protected:

  const Quantity batch_size;
  shared_ptr<EventQueue> eventQ;
  Quantity taskBuffer;
  Quantity low_water_mark;

  friend Instantiate::Factory;
  weak_ptr<Machine> machine;
  std::vector<Supplier> suppliers;
  std::deque<shared_ptr<DemandOrder>> orders;
  Time startTime;

  virtual bool should_enqueue();

  private:

  Task();
  Task(const Task &);

};

}

#endif //TASK_HPP

