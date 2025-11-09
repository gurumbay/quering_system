#include "sim/device/Device.h"

#include "sim/model/Request.h"

Device::Device(size_t id, std::unique_ptr<IDistribution> distribution)
    : id_(id),
      busy_(false),
      current_request_(nullptr),
      service_distribution_(std::move(distribution)),
      next_service_end_time_(NO_EVENT_TIME) {}

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

double Device::schedule_next_service_end(double current_time) {
  double service_time = service_distribution_->generate();
  next_service_end_time_ = current_time + service_time;
  return next_service_end_time_;
}

double Device::get_next_service_end_time() const {
  return next_service_end_time_;
}

void Device::clear_next_service_end_time() {
  next_service_end_time_ = NO_EVENT_TIME;
}
