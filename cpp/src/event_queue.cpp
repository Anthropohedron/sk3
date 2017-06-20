#include <cassert>
#include "event_queue.hpp"
#include "logger.hpp"

namespace SK3 {

bool FuncEntry::operator<(const FuncEntry &rhs) const {
  // this is deliberately reversed
  return time > rhs.time;
}

EventQueue::EventQueue(Logger *_logger, const Time _endtime
    ): logger(NULL), endtime(_endtime), curtime(0) {
  set_logger(_logger);
}

EventQueue::~EventQueue() {
  delete logger;
}

void EventQueue::set_logger(Logger *_logger) {
  if (logger) {
    delete logger;
  }
  logger = _logger;
  if (logger) {
    logger->eventQ = this;
  }
}

void EventQueue::add_event(Time delay, Func *func) {
  assert(delay > 0);
  queue.push(FuncEntry(delay + curtime, func));
}

void EventQueue::run() {
  if (!logger) {
    set_logger(new SimpleLogger());
  }
}

}

