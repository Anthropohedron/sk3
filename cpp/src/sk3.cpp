#include <cmath>
#include <iostream>
#include <iomanip>
#include "sk3.hpp"

namespace SK3 {

using namespace std;

static const int TimeDecimals = 3;
static const int QuantityDecimals = 3;

static const long TimeFactor = 1000L;
static const long QuantityFactor = 1000L;

void time_format(ostream &out, Time time) {
  out << (time / TimeFactor) << '.'
    << setfill('0') << setw(TimeDecimals) << (time % TimeFactor);
}

void quantity_format(ostream &out, Quantity quantity) {
  out << (quantity / QuantityFactor) << '.'
    << setfill('0') << setw(QuantityDecimals) << (quantity % QuantityFactor);
}

Time to_internal_time(double time) {
  return lround(time * TimeFactor);
}

Quantity to_internal_quantity(double quantity) {
  return lround(quantity * QuantityFactor);
}

Quantity internal_quantity_mul(Quantity q1, Quantity q2) {
  return (q1 * q2) / QuantityFactor;
}

}

static inline void usage(char *prog) {
  std::cerr << "Usage: " << prog << " <time limit> <config>" << std::endl;
}

extern "C" int
main(int argc, char **argv) {
  if (argc != 3) {
    usage(*argv);
    return 1;
  }
  SK3::Config::read(argv[2]);
  return 0;
}

