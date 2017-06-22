#include <cassert>
#include "event_queue.hpp"
#include "logger.hpp"

namespace SK3 {

using namespace std;

bool EventQueue::FuncEntry::operator<(const FuncEntry &rhs) const {
  // this is deliberately reversed
  return time > rhs.time;
}

EventQueue::EventQueue(shared_ptr<Logger> _logger
    ): logger(_logger), curTime(0) {
  if (logger) {
    logger->eventQ = this;
  }
}

EventQueue::~EventQueue() { }

void EventQueue::add_event(Time delay, const Func &func) {
  assert(delay > 0);
  queue.push(FuncEntry(delay + curTime, func));
}

void EventQueue::set_logger(shared_ptr<Logger> _logger) {
  logger = _logger;
  if (logger) {
    logger->eventQ = this;
  }
}

bool EventQueue::runOneBefore(Time endTime) {
  if (queue.empty() || (curTime >= endTime)) return false;
  FuncEntry entry = queue.top();
  if (entry.time >= endTime) return false;
  curTime = entry.time;
  entry();
  queue.pop();
  return true;
}

void EventQueue::init_sim() {
  //TODO
}

}

