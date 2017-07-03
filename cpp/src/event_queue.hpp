#ifndef EVENT_QUEUE_HPP
#define EVENT_QUEUE_HPP

#include <queue>
#include "sk3.hpp"
#include "logger.hpp"

namespace SK3 {

namespace Instantiate {
class Factory;
}

class EventQueue :
public SimulationComponent {

  public:

  struct FuncEntry {

    bool operator<(const FuncEntry &rhs) const;
    inline void operator()() { func(); }

    inline FuncEntry(const Time _time,
        Func _func): time(_time), func(_func) {}

    private:

    friend EventQueue;
    Time time;
    Func func;

  };

  EventQueue(shared_ptr<Logger> _logger);
  ~EventQueue();

  inline const Time now() const { return curTime; }

  virtual void init_sim();

  void add_event(Time delay, const Func &func);
  bool runOneBefore(Time endTime);

  struct LogRecord {

    typedef Logger::Record::Type Type;

    inline LogRecord(Type _type, Time _duration,
        const std::string &_details = ""): type(_type),
           duration(_duration), details(_details) { }

    Type type;
    Time duration;
    std::string details;
  };

  inline void log(const LogRecord &record, const LogReporter &reporter) {
    logger->log(
        Logger::Record(curTime, record.type,
          record.duration, record.details),
        reporter);
  }

  private:

  shared_ptr<Logger> logger;
  Time curTime;
  std::priority_queue<FuncEntry> queue;

  EventQueue();
  EventQueue(const EventQueue &);

};

}

#endif //EVENT_QUEUE_HPP

