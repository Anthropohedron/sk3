#include <fstream>
#include <algorithm>
#include "logger_impl.hpp"
#include "config/representation.hpp"

namespace SK3 {

using namespace std;

static const char *const LogRecordType[] = {
  "BufferChg ",
  "ActiveEnd ",
  "IdleEnd   ",
  "RippleEnd ",
  "BufferFull"
};

shared_ptr<Logger> Logger::create(const Config::Logger &cfg) {
  double limit = (cfg.limit <= 0) ? 1000000 : cfg.limit;
  if (cfg.dir.empty()) {
    if (cfg.file.empty()) {
      return make_shared<SimpleLogger>(to_internal_time(limit));
    } else {
      return make_shared<SimpleFileLogger>(to_internal_time(limit),
          cfg.file);
    }
  } else {
    return make_shared<SplitLogger>(limit, cfg.dir);
  }
}

Logger::Logger() {}
Logger::~Logger() {}

void Logger::log_line(ostream &out, const Record &record,
    const LogReporter &reporter) {
  if ((record.type < Record::Type::LOG_DEFICIT) ||
      (record.type > Record::Type::LOG_BUFFER_FULL)) {
    return;
  }
  time_format(out, record.now);
  out << " (";
  time_format(out, record.duration);
  out << "): " << LogRecordType[record.type] << " (" << reporter.name();
  if (record.type == Record::Type::LOG_DEFICIT) {
    out << "deficit: ";
    quantity_format(out, reporter.buffer());
  }
  if (!record.details.empty()) {
    out << ' ' << record.details;
  }
  out << endl;
}

SimpleLogger::SimpleLogger(Time pause, ostream &_out):
  out(_out),
  pause_interval(pause),
  next_pause(pause) { }

SimpleLogger::~SimpleLogger() { }

void SimpleLogger::log(const Record &record,
    const LogReporter &reporter) {
  log_line(out, record, reporter);
  if (record.now >= next_pause) {
    string response = "Y";
    cerr << "Continue (Y/n)? " << flush;
    cin >> response;
    switch (response[0]) {
      case 'n':
      case 'N':
      case 'q':
      case 'Q':
      case 'x':
      case 'X':
        //TODO: throw
        ;
    }
    next_pause += pause_interval;
  }
}

SimpleFileLogger::SimpleFileLogger(Time pause, shared_ptr<ostream> _out):
  SimpleLogger(pause, *_out), outp(_out) { }

SimpleFileLogger::SimpleFileLogger(Time pause, const string &file):
  SimpleFileLogger(pause, make_shared<ofstream>(file)) { }

SimpleFileLogger::~SimpleFileLogger() { }

SplitLogger::SplitLogger(long count_limit, const string &output_dir
    ): limit(count_limit), outdir(output_dir + "/") {}

SplitLogger::~SplitLogger() { }

ostream &SplitLogger::out(const string &key) {
  auto found = streams.find(key);
  if (found == streams.end()) {
    const string fname = outdir + key + ".log";
    shared_ptr<ostream> file = make_shared<ofstream>(fname);
    streams[key] = file;
    return *file;
  } else {
    return *(found->second);
  }
}

// true means stop
void SplitLogger::increment(const string &key) {
  if (!limit) return;
  auto found = counts.find(key);
  long newVal = (found == counts.end()) ? limit : found->second;
  counts[key] = --newVal;
  if (newVal < 1) {
    //TODO: throw
  }
}

void SplitLogger::log(const Record &record,
    const LogReporter &reporter) {
  const string &key = reporter.name();
  log_line(out(key), record, reporter);
  increment(key);
}

}

