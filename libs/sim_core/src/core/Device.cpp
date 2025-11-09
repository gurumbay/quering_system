#include "sim/core/Device.h"

#include "sim/core/Request.h"

Device::Device(size_t id)
    : id_(id),
      busy_(false),
      current_request_(nullptr) {}

bool Device::is_free() const { return !busy_; }

void Device::start_service(std::shared_ptr<Request> request, double now) {
  busy_ = true;
  request->set_service_start_time(now);
  current_request_ = request;
}

std::shared_ptr<Request> Device::finish_service() {
  if (busy_) {
    busy_ = false;
    std::shared_ptr<Request> finished_request = current_request_;
    current_request_ = nullptr;
    return finished_request;
  }
  return nullptr;
}

size_t Device::get_id() const { return id_; }

std::shared_ptr<Request> Device::get_current_request() const {
  return current_request_;
}
