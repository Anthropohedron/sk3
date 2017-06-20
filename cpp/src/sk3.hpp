#ifndef SK3_HPP
#define SK3_HPP

#include <memory>
#include <string>
#include <ostream>

namespace std {

// why isn't this in the standard library?

template<typename T>
constexpr inline bool operator<(weak_ptr<T> lhs, weak_ptr<T> rhs) {
  return lhs.lock().get() < rhs.lock().get();
}

template<typename T>
constexpr inline bool operator==(weak_ptr<T> lhs, weak_ptr<T> rhs) {
  return lhs.lock().get() == rhs.lock().get();
}

} // namespace std

namespace SK3 {

using std::shared_ptr;
using std::weak_ptr;

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

