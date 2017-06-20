#ifndef DEMAND_HPP
#define DEMAND_HPP

#include "sk3.hpp"

namespace SK3 {

class Demand : public SimulationComponent {
  
  public:

  Demand(EventQueue &_eventQ, Task &_task,
      Quantity _quantity, Time _interval, Time _offset);

  protected:

  EventQueue &eventQ;
  Task &task;
  const Quantity quantity;
  const Time interval;
  const Time offset;
  long counter;

};

}

#endif //DEMAND_HPP

