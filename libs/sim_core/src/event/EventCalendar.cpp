#include "sim/event/EventCalendar.h"

#include "sim/event/Event.h"

EventCalendar::EventCalendar() {}

void EventCalendar::schedule(const Event& event) {
  events_.push(event);
}

Event EventCalendar::pop_next() {
  Event event = events_.top();
  events_.pop();
  return event;
}

double EventCalendar::get_next_time() const {
  if (events_.empty()) {
    return NO_EVENT_TIME;
  }
  return events_.top().get_time();
}

size_t EventCalendar::get_size() const { return events_.size(); }

bool EventCalendar::is_empty() const { return events_.empty(); }
