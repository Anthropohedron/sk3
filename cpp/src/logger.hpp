#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <map>
#include <string>
#include <iostream>
#include "sk3.hpp"

namespace SK3 {

class EventQueue;

enum LogType {
  LOG_DEFICIT,
  LOG_ACTIVE_END,
  LOG_IDLE_END,
  LOG_RIPPLE_END,
  LOG_BUFFER_FULL
};

struct LogReporter {
  virtual const std::string name() const = 0;
  virtual const Quantity buffer() const = 0;
};

class Logger {

  public:

  virtual void log(const LogType type, const LogReporter &reporter,
      const Time length, const std::string &details = "") = 0;

  virtual ~Logger();

  protected:

  Logger();

  void log_line(std::ostream &out,
      const LogType type, const LogReporter &reporter,
      const Time length, const std::string &details);

  friend EventQueue;
  EventQueue *eventQ;

};

class SimpleLogger : public Logger {

  public:

  SimpleLogger(Time pause = 1000000, std::ostream &_out = std::cout);
  SimpleLogger(Time pause, std::ostream *_out);
  SimpleLogger(Time pause, const std::string &file);

  virtual ~SimpleLogger();

  virtual void log(const LogType type, const LogReporter &reporter,
      const Time length, const std::string &details = "");

  private:

  std::ostream &out;
  std::ostream *outp;
  const Time pause_interval;
  Time next_pause;

};

class SplitLogger : public Logger {

  public:

  SplitLogger(long count_limit = 1000000,
      const std::string &output_dir = "/tmp");

  virtual ~SplitLogger();

  virtual void log(const LogType type, const LogReporter &reporter,
      const Time length, const std::string &details = "");

  private:

  std::ostream &out(const std::string &key);
  void increment(const std::string &key);

  std::map<std::string, std::ostream *> streams;
  std::map<std::string, long> counts;

  const long limit;
  const std::string outdir;

};

}

#endif //LOGGER_HPP

