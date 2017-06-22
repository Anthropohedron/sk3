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

  struct Record {

    enum Type {
      LOG_DEFICIT,
      LOG_ACTIVE_END,
      LOG_IDLE_END,
      LOG_RIPPLE_END,
      LOG_BUFFER_FULL
    };

    inline Record(Time _now, Type _type, Time _duration,
        const std::string &_details = ""): now(_now), type(_type),
           duration(_duration), details(_details) { }

    Time now;
    Type type;
    Time duration;
    std::string details;
  };

  typedef Config::Logger config_type;

  static shared_ptr<Logger> create(const Config::Logger &cfg);

  virtual ~Logger();

  protected:

  Logger();

  void log_line(std::ostream &out, const Record &record,
      const LogReporter &reporter);

  friend EventQueue;
  virtual void log(const Record &record, const LogReporter &reporter) = 0;

};

}

#endif //LOGGER_HPP

