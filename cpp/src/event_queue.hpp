#ifndef EVENT_QUEUE_HPP
#define EVENT_QUEUE_HPP

#include <queue>
#include "sk3.hpp"

namespace SK3 {

class Logger;

struct Func {
  virtual void operator()() = 0;
  virtual ~Func() {};
};

class EventQueue;

struct FuncEntry {

  bool operator<(const FuncEntry &rhs) const;
  inline void operator()() { (*func)(); }

  inline FuncEntry(const Time _time,
      shared_ptr<Func> _func): time(_time), func(_func) {}

  private:

  friend EventQueue;
  Time time;
  shared_ptr<Func> func;

};

namespace Instantiate {
class Factory;
}

class EventQueue : public SimulationComponent {

  public:

  EventQueue(shared_ptr<Logger> _logger = NULL);
  ~EventQueue();

  inline const Time now() const { return curTime; }

  virtual void init_sim();

  void add_event(Time delay, Func *func);
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

