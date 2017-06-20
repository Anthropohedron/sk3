#ifndef CONFIG_READER_HPP
#define CONFIG_READER_HPP

#include <yaml-cpp/node/convert.h>
#include <string>
#include <vector>
#include <map>
#include "sk3.hpp"

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

template<class T> struct CEImpl {
  CEImpl(const std::map<std::string, Task> &tasks); // never defined
  int countErrors(const T &val); // never defined
};

template<class T> struct CountErrors {

  CountErrors(const std::map<std::string, Task> &tasks): impl(tasks) { }

  inline int operator()(int sum,
      const std::pair<const std::string, T> &val) {
    return sum + impl.countErrors(val.second);
  }

  private:

  CEImpl<T> impl;

};

template<> struct CEImpl<Task> {
  inline CEImpl(const std::map<std::string, Task> &tasks) { }
  int countErrors(const Task &task);
};

template<> struct CEImpl<Machine> {

  inline CEImpl(const std::map<std::string, Task> &tasks): knownTasks(tasks) { }
  int countErrors(const Machine &machine);

  private:

  const std::map<std::string, Task> &knownTasks;
  // Task -> Machine
  std::map<std::string, std::string> assignedTasks;

};

template<> struct CEImpl<Demand> {

  inline CEImpl(const std::map<std::string, Task> &tasks): knownTasks(tasks) { }
  int countErrors(const Demand &demand);

  private:

  const std::map<std::string, Task> &knownTasks;
  // Destination -> Source
  std::map<std::string, std::string> edges;

};

}

}
}

namespace YAML {

namespace SK3C {
  using namespace SK3::Config;
}

template<> struct convert<SK3C::Logger> {
  static bool decode(const Node &node, SK3C::Logger &rhs);
};

template<> struct convert<SK3C::Variants> {
  static bool decode(const Node &node, SK3C::Variants &rhs);
};

template<> struct convert<SK3C::Task> {
  static bool decode(const Node &node, SK3C::Task &rhs);
};

template<> struct convert<SK3C::Machine> {
  static bool decode(const Node &node, SK3C::Machine &rhs);
};

template<> struct convert<SK3C::Demand> {
  static bool decode(const Node &node, SK3C::Demand &rhs);
};

template<> struct convert<SK3C::Config> {
  static bool decode(const Node &node, SK3C::Config &rhs);
};

}

#endif //CONFIG_READER_HPP

