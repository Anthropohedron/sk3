#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>
#include "sk3.hpp"

namespace SK3 {

class EventQueue;

struct LogReporter {
  virtual const std::string name() const = 0;
  virtual const Quantity buffer() const = 0;
};

namespace Config {
struct Logger;
}

class Logger {

  public:

  enum LogType {
    LOG_DEFICIT,
    LOG_ACTIVE_END,
    LOG_IDLE_END,
    LOG_RIPPLE_END,
    LOG_BUFFER_FULL
  };

  typedef Config::Logger config_type;

  static shared_ptr<Logger> create(const Config::Logger &cfg);

  virtual ~Logger();

  protected:

  Logger();

  void log_line(std::ostream &out, Time now,
      const LogType type, const LogReporter &reporter,
      const Time length, const std::string &details);

  friend EventQueue;
  virtual void log(Time now, const LogType type,
      const LogReporter &reporter,
      const Time length, const std::string &details = "") = 0;

};

}

#endif //LOGGER_HPP

