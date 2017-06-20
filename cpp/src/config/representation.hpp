#ifndef CONFIG__REPRESENTATION_HPP
#define CONFIG__REPRESENTATION_HPP

#include <string>
#include <vector>
#include <map>

namespace SK3 {
namespace Config {

struct Logger {
  std::string file;
  std::string dir;
  long limit;
};

struct Variants {
  std::string task;
  std::string machine;
  std::string demand;
};

struct Task {
  std::string name;
  double batch_time;
  double batch_size;
  double deficit;
};

struct Machine {
  std::string name;
  std::vector<std::string> tasks;
};

struct Demand {
  std::string name;
  std::string destination;
  double quantity;
  double interval;
  double offset;
};

struct Config {
  Logger logger;
  Variants variants;
  std::map<std::string, Task> tasks;
  std::map<std::string, Machine> machines;
  std::map<std::string, Demand> demands;
};

namespace Validate {
int countErrors(const Config &config);
}

} // namespace SK3::Config
} // namespace SK3

#endif //CONFIG__REPRESENTATION_HPP

