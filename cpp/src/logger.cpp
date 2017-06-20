#include <fstream>
#include <iostream>
#include <algorithm>
#include "logger.hpp"
#include "event_queue.hpp"

namespace SK3 {

using namespace std;

static const char *const LogRecordType[] = {
  "BufferChg ",
  "ActiveEnd ",
  "IdleEnd   ",
  "RippleEnd ",
  "BufferFull"
};

Logger::Logger() {}
Logger::~Logger() {}

void Logger::log_line(ostream &out,
    const LogType type, const LogReporter &reporter,
    const Time length, const string &details) {
  if ((type < LOG_DEFICIT) || (type > LOG_BUFFER_FULL)) {
    return;
  }
  time_format(out, eventQ->now());
  out << " (";
  time_format(out, length);
  out << "): " << LogRecordType[type] << " (" << reporter.name();
  if (type == LOG_DEFICIT) {
    out << "deficit: ";
    quantity_format(out, reporter.buffer());
  }
  if (!details.empty()) {
    out << ' ' << details;
  }
  out << endl;
}

SimpleLogger::SimpleLogger(Time pause, ostream &_out
    ): out(_out), pause_interval(pause), next_pause(pause) {}

SimpleLogger::~SimpleLogger() {}

void SimpleLogger::log(const LogType type, const LogReporter &reporter,
    const Time length, const string &details) {
  log_line(out, type, reporter, length, details);
  if (eventQ->now() >= next_pause) {
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

SplitLogger::SplitLogger(long count_limit, const string &output_dir
    ): limit(count_limit), outdir(output_dir + "/") {}

SplitLogger::~SplitLogger() {
  for_each(streams.begin(), streams.end(),
      [](pair<const string, ostream *> &val) {
        ofstream *file = dynamic_cast<ofstream *>(val.second);
        file->close();
        delete file;
      });
  streams.clear();
}

ostream &SplitLogger::out(const string &key) {
  auto found = streams.find(key);
  if (found == streams.end()) {
    const string fname = outdir + key + ".log";
    ostream *file = new ofstream(fname);
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

void SplitLogger::log(const LogType type, const LogReporter &reporter,
    const Time length, const string &details) {
  const string &key = reporter.name();
  log_line(out(key), type, reporter, length, details);
  increment(key);
}

}

