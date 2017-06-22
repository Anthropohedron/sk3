#ifndef LOGGER_IMPL_HPP
#define LOGGER_IMPL_HPP

#include "logger.hpp"

namespace SK3 {

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

  StringPtrMap<std::ostream> streams;
  StringMap<long> counts;

  const long limit;
  const std::string outdir;

};

}

#endif //LOGGER_IMPL_HPP

