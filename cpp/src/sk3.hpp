#ifndef SK3_HPP
#define SK3_HPP

#include <string>
#include <ostream>

namespace SK3 {

class System;

typedef long Time;
typedef long Quantity;

namespace Config {

System *read(const std::string &filename);

}

struct SimulationComponent {
  virtual void init_sim() = 0;
};

void time_format(std::ostream &out, Time time);
void quantity_format(std::ostream &out, Quantity quantity);

Time to_internal_time(double time);
Quantity to_internal_quantity(double quantity);

Quantity internal_quantity_mul(Quantity q1, Quantity q2);

}

#endif //SK3_HPP

