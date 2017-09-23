#ifndef FIXEDPOINT_HPP
#define FIXEDPOINT_HPP

#include <cmath>
#include <ostream>
#include <iomanip>

namespace SK3 {

template <int Precision>
struct FixedPoint {

  enum {
    Scale = 10 * FixedPoint<Precision-1>::Scale
  };

  inline FixedPoint() : value(0) {}
  inline FixedPoint(double val) : value(lround(val * Scale)) {}

  inline constexpr FixedPoint operator*(long factor) const {
    return FixedPoint(value * factor);
  }

  inline constexpr FixedPoint operator*(const FixedPoint &factor) const {
    return FixedPoint((value * factor.value) / Scale);
  }

  inline constexpr FixedPoint operator+(const FixedPoint &add) const {
    return FixedPoint(value + add.value);
  }

  inline const FixedPoint &operator+=(const FixedPoint &add) {
    value += add.value;
    return *this;
  }

  inline constexpr FixedPoint operator-(const FixedPoint &sub) const {
    return FixedPoint(value - sub.value);
  }

  inline const FixedPoint &operator-=(const FixedPoint &sub) {
    value -= sub.value;
    return *this;
  }

  inline constexpr FixedPoint operator-() const {
    return FixedPoint(-value);
  }

  inline bool operator<(int cmp) {
    return value < cmp;
  }

  inline bool operator<(const FixedPoint &cmp) {
    return value < cmp.value;
  }

  inline constexpr operator long() const {
    return value;
  }

  inline void format(std::ostream &os) const {
    const long whole   = value / Scale;
    const long decimal = value % Scale;
    os << whole << '.' <<
      std::setfill('0') << std::setw(Precision) << decimal;
  }

  private:

  long value;
  inline FixedPoint(long val, bool _) : value(val) {}

};

template <int Precision> inline
std::ostream &operator<<(std::ostream &os,
    const FixedPoint<Precision> &fp) {
  fp.format(os);
  return os;
}

template <>
struct FixedPoint<0> {

  enum {
    Scale = 1
  };

};

} // namespace SK3

#endif //FIXEDPOINT_HPP

