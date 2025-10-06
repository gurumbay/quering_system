#ifndef SIM_CORE_EVENT_H_
#define SIM_CORE_EVENT_H_

#include <cstddef>

enum class EventType {
    arrival,
    service_end
};

class Event {
public:
    Event(double time, EventType type, size_t request_id, size_t device_id = 0, size_t source_id = 0);
    double get_time() const;
    EventType get_type() const;
    size_t get_request_id() const;
    size_t get_device_id() const;
    size_t get_source_id() const;

private:
    double time_;
    EventType type_;
    size_t request_id_;
    size_t device_id_;
    size_t source_id_;
};

bool operator<(const Event& lhs, const Event& rhs);

#endif // SIM_CORE_EVENT_H_