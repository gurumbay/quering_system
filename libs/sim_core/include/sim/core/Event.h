#ifndef SIM_CORE_EVENT_H_
#define SIM_CORE_EVENT_H_

#include <cstddef>
#include <memory>

class Request;
class Device;

enum class EventType { arrival, service_end };

class Event {
 public:
  Event(double time, EventType type, std::weak_ptr<Request> request,
        Device* device = nullptr, size_t source_id = 0);
  double get_time() const;
  EventType get_type() const;
  std::weak_ptr<Request> get_request() const;
  Device* get_device() const;
  size_t get_request_id() const;
  size_t get_device_id() const;
  size_t get_source_id() const;

 private:
  double time_;
  EventType type_;
  std::weak_ptr<Request> request_;
  Device* device_;  // Non-owning pointer
  size_t source_id_;
};

bool operator<(const Event& lhs, const Event& rhs);

#endif  // SIM_CORE_EVENT_H_