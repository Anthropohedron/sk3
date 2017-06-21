#ifndef DEMAND_HPP
#define DEMAND_HPP

#include "sk3.hpp"
#include "config/representation.hpp"

namespace SK3 {

class EventQueue;

class Demand : public SimulationComponent {
  
  public:

  typedef Config::Demand config_type;

  static demand_factory_t factoryFor(const std::string &variant);

  Demand(shared_ptr<EventQueue> _eventQ, weak_ptr<Task> _task,
      Quantity _quantity, Time _interval, Time _offset);

  virtual void init_sim();

  protected:

  shared_ptr<EventQueue> eventQ;
  weak_ptr<Task> task;
  const Quantity quantity;
  const Time interval;
  const Time offset;
  long counter;

};

}

#endif //DEMAND_HPP

