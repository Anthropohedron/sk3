#ifndef CONFIG__REPRESENTATION_HPP
#define CONFIG__REPRESENTATION_HPP

#include <memory>
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

using std::shared_ptr;

class EventQueue;

class Task;
class Machine;
class Demand;

typedef std::map<std::string, shared_ptr<Task>> TaskMap;

typedef shared_ptr<Task>    (*task_factory_t   )(const Config::Task    &,
    shared_ptr<EventQueue>);
typedef shared_ptr<Machine> (*machine_factory_t)(const Config::Machine &,
    shared_ptr<EventQueue>, TaskMap &);
typedef shared_ptr<Demand>  (*demand_factory_t )(const Config::Demand  &,
    shared_ptr<EventQueue>, TaskMap &);

} // namespace SK3

#endif //CONFIG__REPRESENTATION_HPP

