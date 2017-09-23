#include <cassert>
#include <iostream>
#include "event_queue.hpp"
#include "logger.hpp"

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#define STATUS_ON_SIGNAL
#include <signal.h>

namespace {

static SK3::EventQueue *g_EventQueue = nullptr;

static void print_status(int signum) {
  if (g_EventQueue) {
    g_EventQueue->print_info();
  }
}

} //anonymous namespace

#endif //STATUS_ON_SIGNAL

namespace SK3 {

using namespace std;

bool EventQueue::FuncEntry::operator<(const FuncEntry &rhs) const {
  // this is deliberately reversed
  return time > rhs.time;
}

EventQueue::EventQueue(shared_ptr<Logger> _logger):
  logger(_logger),
  curTime(0) {
#ifdef STATUS_ON_SIGNAL
    if (!g_EventQueue) {
      g_EventQueue = this;
      signal(SIGUSR1, print_status);
    }
#endif //STATUS_ON_SIGNAL
  }

EventQueue::~EventQueue() {
#ifdef STATUS_ON_SIGNAL
  if (g_EventQueue == this) {
    g_EventQueue = nullptr;
    signal(SIGUSR1, SIG_DFL);
  }
#endif //STATUS_ON_SIGNAL
}

void EventQueue::print_info() {
  cerr << curTime << "\tQueue size: " << queue.size() << endl;
}

void EventQueue::add_event(Time delay, const Func &func) {
  assert(delay > 0);
  queue.push(FuncEntry(delay + curTime, func));
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
  priority_queue<FuncEntry> temp;
  queue.swap(temp);
  curTime = 0;
}

}

