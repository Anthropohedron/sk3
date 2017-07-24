#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "sk3.hpp"
#include "system.hpp"

using namespace std;

namespace SK3 {

halt_simulation::~halt_simulation() { }
const char *halt_simulation::what() const noexcept { return msg.c_str(); }

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

} // namespace SK3

using namespace SK3;

static inline void usage(char *prog) {
  cerr << "Usage: " << prog << " <time limit> <config>" << endl;
}

extern "C" int
main(int argc, char **argv) {
  if (argc != 3) {
    usage(*argv);
    return 1;
  }
  double raw_limit = strtod(argv[1], NULL);
  if (raw_limit <= 0.0) {
    usage(*argv);
    return 2;
  }
  Time limit = to_internal_time(raw_limit);
  shared_ptr<System> simulator = Config::read(argv[2]);
  simulator->runUntil(limit);
  return 0;
}

