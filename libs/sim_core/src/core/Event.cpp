#include "sim/core/Event.h"

Event::Event(double time, EventType type, size_t request_id, size_t device_id,
             size_t source_id)
    : time_(time),
      type_(type),
      request_id_(request_id),
      device_id_(device_id),
      source_id_(source_id) {}

double Event::get_time() const { return time_; }

EventType Event::get_type() const { return type_; }

size_t Event::get_request_id() const { return request_id_; }

size_t Event::get_device_id() const { return device_id_; }

size_t Event::get_source_id() const { return source_id_; }

bool operator<(const Event& lhs, const Event& rhs) {
  if (lhs.get_time() != rhs.get_time()) return lhs.get_time() > rhs.get_time();
  if (lhs.get_type() != rhs.get_type()) return lhs.get_type() > rhs.get_type();
  return lhs.get_request_id() > rhs.get_request_id();
}
