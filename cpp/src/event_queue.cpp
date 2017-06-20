#include <cassert>
#include "event_queue.hpp"
#include "logger.hpp"

namespace SK3 {

using namespace std;

bool FuncEntry::operator<(const FuncEntry &rhs) const {
  // this is deliberately reversed
  return time > rhs.time;
}

EventQueue::EventQueue(shared_ptr<Logger> _logger, const Time _endtime
    ): logger(_logger), endtime(_endtime), curtime(0) {
  if (logger) {
    logger->eventQ = this;
  }
}

EventQueue::~EventQueue() { }

void EventQueue::set_logger(shared_ptr<Logger> _logger) {
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
    set_logger(make_shared<SimpleLogger>());
  }
  //TODO
}

void EventQueue::init_sim() {
  //TODO
}

}

