#ifndef CONFIG__VALIDATION_HPP
#define CONFIG__VALIDATION_HPP

#include <string>
#include <map>
#include "representation.hpp"

namespace SK3 {
namespace Config {
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

} // namespace SK3::Config::Validate
} // namespace SK3::Config
} // namespace SK3

#endif //CONFIG__VALIDATION_HPP

