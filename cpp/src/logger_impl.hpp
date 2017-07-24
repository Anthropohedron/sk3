#ifndef LOGGER_IMPL_HPP
#define LOGGER_IMPL_HPP

#include "logger.hpp"

namespace SK3 {

class SimpleLogger : public Logger {

  public:

  SimpleLogger(Time pause = 1000000, std::ostream &_out = std::cout);

  virtual ~SimpleLogger();

  virtual void log(const Record &record, const LogReporter &reporter);

  private:

  std::ostream &out;
  const Time pause_interval;
  Time next_pause;

};

class SimpleFileLogger : public SimpleLogger {

  public:

  SimpleFileLogger(Time pause, shared_ptr<std::ostream> _out);
  SimpleFileLogger(Time pause, const std::string &file);

  virtual ~SimpleFileLogger();

  private:

  shared_ptr<std::ostream> outp;

};

class SplitLogger : public Logger {

  public:

  SplitLogger(long count_limit = 1000000,
      const std::string &output_dir = "/tmp");

  virtual ~SplitLogger();

  virtual void log(const Record &record, const LogReporter &reporter);

  private:

  std::ostream &out(const std::string &key);
  bool increment(const std::string &key);

  StringPtrMap<std::ostream> streams;
  StringMap<long> counts;

  const long limit;
  const std::string outdir;

};

}

#endif //LOGGER_IMPL_HPP

