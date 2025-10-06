#ifndef SIM_CORE_EVENT_CALENDAR_H_
#define SIM_CORE_EVENT_CALENDAR_H_

#include <queue>
#include <cstddef>
#include "sim/core/Event.h"

class EventCalendar {
public:
    EventCalendar();
    void schedule(const Event& event);
    Event pop_next();
    double get_next_time() const;
    size_t get_size() const;
    bool is_empty() const;

private:
    std::priority_queue<Event> events_;
};

#endif // SIM_CORE_EVENT_CALENDAR_H_
