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
  Time limit = Time(raw_limit);
  shared_ptr<System> simulator = Config::read(argv[2]);
  simulator->runUntil(limit);
  return 0;
}

