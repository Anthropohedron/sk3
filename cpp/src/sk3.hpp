#ifndef SK3_HPP
#define SK3_HPP

#include <memory>
#include <string>
#include <map>
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

template<class V>
using StringMap = std::map<std::string, V>;
template<class V>
using StringPtrMap = StringMap<shared_ptr<V>>;
template<class V>
using StringPtrPair = typename StringPtrMap<V>::value_type;

class System;

typedef long Time;
typedef long Quantity;

namespace Config {

shared_ptr<System> read(const std::string &filename);

} // namespace SK3::Config

struct SimulationComponent {
  virtual void init_sim() = 0;
};

void time_format(std::ostream &out, Time time);
void quantity_format(std::ostream &out, Quantity quantity);

Time to_internal_time(double time);
Quantity to_internal_quantity(double quantity);

Quantity internal_quantity_mul(Quantity q1, Quantity q2);

} // namespace SK3

#endif //SK3_HPP

