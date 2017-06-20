#ifndef CONFIG_IMPL_HPP
#define CONFIG_IMPL_HPP

namespace SK3 {
namespace Config {

struct Logger {
  std::string file;
  std::string dir;
  long rotate;
};

struct Task {
  std::string name;
  Time batch_time;
  Quantity batch_size;
  Quantity deficit;
};

struct Machine {
  std::string name;
  std::vector<string> tasks;
};

struct Demand {
  std::string src;
  std::string dst;
  Quantity quantity;
  Time interval;
  Time offset;
};

}
}

#endif //CONFIG_IMPL_HPP

