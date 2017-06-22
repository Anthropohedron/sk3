#ifndef CONFIG__VALIDATION_HPP
#define CONFIG__VALIDATION_HPP

#include <string>
#include <map>
#include "representation.hpp"

namespace SK3 {
namespace Config {
namespace Validate {

template<class T> struct CEImpl {
  CEImpl(const StringMap<Task> &tasks); // never defined
  int countErrors(const T &val); // never defined
};

template<class T> struct CountErrors {

  CountErrors(const StringMap<Task> &tasks): impl(tasks) { }

  inline int operator()(int sum,
      const std::pair<const std::string, T> &val) {
    return sum + impl.countErrors(val.second);
  }

  inline int operator()(int sum, const T &val) {
    return sum + impl.countErrors(val);
  }

  private:

  CEImpl<T> impl;

};

template<> struct CEImpl<Task> {
  inline CEImpl(const StringMap<Task> &tasks) { }
  int countErrors(const Task &task);
};

template<> struct CEImpl<Machine> {

  inline CEImpl(const StringMap<Task> &tasks): knownTasks(tasks) { }
  int countErrors(const Machine &machine);

  private:

  const StringMap<Task> &knownTasks;
  // Task -> Machine
  StringMap<std::string> assignedTasks;

};

template<> struct CEImpl<Demand> {

  inline CEImpl(const StringMap<Task> &tasks): knownTasks(tasks) { }
  int countErrors(const Demand &demand);

  private:

  const StringMap<Task> &knownTasks;
  // Destination -> Source
  StringMap<std::string> edges;

};

} // namespace SK3::Config::Validate
} // namespace SK3::Config
} // namespace SK3

#endif //CONFIG__VALIDATION_HPP

