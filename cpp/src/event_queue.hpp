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

struct FuncEntry {

  bool operator<(const FuncEntry &rhs) const;
  inline void operator()() { (*func)(); }
  ~FuncEntry() { delete func; }

  inline FuncEntry(const Time _time, Func *_func): time(_time), func(_func) {}

  private:

  Time time;
  Func *func;

};

class EventQueue : public SimulationComponent {

  public:

  EventQueue(shared_ptr<Logger> _logger = NULL, const Time _endtime = 0);
  ~EventQueue();

  inline const Time now() const { return curtime; }
  inline const Time end() const { return endtime; }

  virtual void init_sim();

  void add_event(Time delay, Func *func);
  void run();

  private:

  void set_logger(shared_ptr<Logger> _logger);

  shared_ptr<Logger> logger;
  Time endtime;
  Time curtime;
  std::priority_queue<FuncEntry> queue;

};

}

#endif //EVENT_QUEUE_HPP

