#include "sim/core/Event.h"

#include "sim/core/Device.h"
#include "sim/core/Request.h"

Event::Event(double time, EventType type, std::weak_ptr<Request> request,
             Device* device, size_t source_id)
    : time_(time),
      type_(type),
      request_(request),
      device_(device),
      source_id_(source_id) {}

double Event::get_time() const { return time_; }

EventType Event::get_type() const { return type_; }

std::weak_ptr<Request> Event::get_request() const { return request_; }

Device* Event::get_device() const { return device_; }

size_t Event::get_request_id() const {
  auto request = request_.lock();
  return request ? request->get_id() : 0;
}

size_t Event::get_device_id() const { return device_ ? device_->get_id() : 0; }

size_t Event::get_source_id() const { return source_id_; }

bool operator<(const Event& lhs, const Event& rhs) {
  if (lhs.get_time() != rhs.get_time()) return lhs.get_time() > rhs.get_time();
  if (lhs.get_type() != rhs.get_type()) return lhs.get_type() > rhs.get_type();
  return lhs.get_request_id() > rhs.get_request_id();
}
