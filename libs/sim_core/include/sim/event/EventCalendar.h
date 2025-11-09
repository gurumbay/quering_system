#ifndef SIM_EVENT_EVENT_CALENDAR_H_
#define SIM_EVENT_EVENT_CALENDAR_H_

#include <cstddef>
#include <queue>

#include "sim/event/Event.h"

class EventCalendar {
 public:
  static constexpr double NO_EVENT_TIME = -1.0;

  EventCalendar();
  void schedule(const Event& event);
  Event pop_next();
  double get_next_time() const;
  size_t get_size() const;
  bool is_empty() const;

 private:
  std::priority_queue<Event> events_;
};

#endif  // SIM_EVENT_EVENT_CALENDAR_H_
