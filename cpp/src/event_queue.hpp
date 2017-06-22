#ifndef EVENT_QUEUE_HPP
#define EVENT_QUEUE_HPP

#include <queue>
#include "sk3.hpp"

namespace SK3 {

class Logger;

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

  EventQueue(shared_ptr<Logger> _logger = NULL);
  ~EventQueue();

  inline const Time now() const { return curTime; }

  virtual void init_sim();

  void add_event(Time delay, const Func &func);
  bool runOneBefore(Time endTime);

  private:

  friend Instantiate::Factory;
  void set_logger(shared_ptr<Logger> _logger);

  shared_ptr<Logger> logger;
  Time curTime;
  std::priority_queue<FuncEntry> queue;

};

}

#endif //EVENT_QUEUE_HPP

