#ifndef DEMAND_HPP
#define DEMAND_HPP

#include "sk3.hpp"
#include "logger.hpp"
#include "config/representation.hpp"

namespace SK3 {

class EventQueue;
class DemandOrder;

class Demand :
public std::enable_shared_from_this<Demand>,
public SimulationComponent {

  public:

  typedef DemandOrder Order;
  typedef Config::Demand config_type;

  static demand_factory_t factoryFor(const std::string &variant);

  Demand(shared_ptr<EventQueue> _eventQ, weak_ptr<Task> _task,
      Quantity _quantity, Time _interval, Time _offset);

  virtual void init_sim();

  protected:

  friend Order;

  virtual void sendOrder();

  shared_ptr<EventQueue> eventQ;
  weak_ptr<Task> task;
  const Quantity quantity;
  const Time interval;
  const Time offset;
  long counter;

  private:

  Func intervalFunc;

  void onInterval();

  Demand();
  Demand(const Demand &);

};

class DemandOrder : public LogReporter {

  public:

  DemandOrder(shared_ptr<Demand> _demand, long _serial);

  void taking(weak_ptr<Task> task, Quantity quantity);
  std::pair<bool, Quantity> fulfilling(weak_ptr<Task> task,
      Quantity quantity);

  inline bool isComplete() { return products.empty(); }

  virtual const std::string &name() const;
  virtual const Quantity buffer() const;

  private:

  shared_ptr<EventQueue> eventQ;
  const Time startTime;
  const std::string demandName;
  const std::string serial;
  std::map<weak_ptr<Task>, Quantity> products;

  DemandOrder();
  DemandOrder(const DemandOrder &);

};


}

#endif //DEMAND_HPP

