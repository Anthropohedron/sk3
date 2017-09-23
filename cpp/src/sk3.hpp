#ifndef SK3_HPP
#define SK3_HPP

#include <functional>
#include <memory>
#include <string>
#include <map>
#include <exception>
#include "fixedpoint.hpp"

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

typedef std::function<void()> Func;

template<class V>
using StringMap = std::map<std::string, V>;
template<class V>
using StringPtrMap = StringMap<shared_ptr<V>>;
template<class V>
using StringPtrPair = typename StringPtrMap<V>::value_type;

class System;

typedef FixedPoint<3> Time;
typedef FixedPoint<3> Quantity;

struct halt_simulation : public std::exception {

  inline halt_simulation(Time _end, const std::string &_msg):
    end(_end), msg(_msg) { }
  inline halt_simulation(const halt_simulation &other):
    end(other.end), msg(other.msg) { }

  inline halt_simulation &operator=(const halt_simulation &other) {
    end = other.end;
    msg = other.msg;
    return *this;
  }

  virtual ~halt_simulation();
  virtual const char *what() const noexcept;

  Time end;
  std::string msg;
};

namespace Config {

shared_ptr<System> read(const std::string &filename);

} // namespace SK3::Config

struct SimulationComponent {
  virtual void init_sim() = 0;
};

} // namespace SK3

#endif //SK3_HPP

