#ifndef CONFIG__REPRESENTATION_HPP
#define CONFIG__REPRESENTATION_HPP

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "../sk3.hpp"

namespace SK3 {

class Logger;
class Task;
class Machine;
class Demand;

namespace Config {

struct Logger {
  static const std::string NAME;
  std::string file;
  std::string dir;
  double limit;
};

struct Variants {
  static const std::string NAME;
  std::string task;
  std::string machine;
  std::string demand;
};

struct Task {
  static const std::string NAME;
  std::string name;
  double batch_time;
  double batch_size;
  double deficit;
};

struct Machine {
  static const std::string NAME;
  std::string name;
  std::vector<std::string> tasks;
};

struct Demand {
  static const std::string NAME;
  std::string name;
  std::string destination;
  double quantity;
  double interval;
  double offset;
};

struct Config {
  static const std::string NAME;
  Logger logger;
  Variants variants;
  StringMap<Task> tasks;
  StringMap<Machine> machines;
  StringMap<Demand> demands;
};

namespace Validate {
int countErrors(const Config &config);
}

} // namespace SK3::Config

using std::shared_ptr;

class EventQueue;

typedef StringPtrMap<Task> TaskMap;

typedef shared_ptr<Task>    (*task_factory_t   )(const Config::Task    &,
    shared_ptr<EventQueue>);
typedef shared_ptr<Machine> (*machine_factory_t)(const Config::Machine &,
    shared_ptr<EventQueue>, TaskMap &);
typedef shared_ptr<Demand>  (*demand_factory_t )(const Config::Demand  &,
    shared_ptr<EventQueue>, TaskMap &);

} // namespace SK3

#endif //CONFIG__REPRESENTATION_HPP

