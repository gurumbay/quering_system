#include "sim/core/Device.h"

Device::Device(size_t id) 
    : id_(id), busy_(false), current_request_id_(0), busy_since_(0.0), total_busy_time_(0.0) {
}

bool Device::is_free() const {
    return !busy_;
}

void Device::start_service(size_t request_id, double now) {
    busy_ = true;
    current_request_id_ = request_id;
    busy_since_ = now;
}

size_t Device::finish_service(double now) {
    if (busy_) {
        total_busy_time_ += (now - busy_since_);
        busy_ = false;
        size_t finished_id = current_request_id_;
        current_request_id_ = 0;
        return finished_id;
    }
    return 0;
}

double Device::get_utilization(double total_time) const {
    if (total_time <= 0.0) return 0.0;
    return total_busy_time_ / total_time;
}

size_t Device::get_id() const {
    return id_;
}

size_t Device::get_current_request_id() const {
    return current_request_id_;
}
