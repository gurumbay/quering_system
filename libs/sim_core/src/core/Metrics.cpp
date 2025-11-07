#include "sim/core/Metrics.h"

#include <algorithm>

Metrics::Metrics()
    : arrived_(0),
      refused_(0),
      completed_(0),
      sum_time_in_system_(0.0),
      sum_waiting_time_(0.0),
      sum_service_time_(0.0) {}

void Metrics::record_arrival(size_t source_id) {
  ++arrived_;
  if (source_id >= source_arrivals_.size()) {
    source_arrivals_.resize(source_id + 1, 0);
  }
  ++source_arrivals_[source_id];
}

void Metrics::record_refusal(size_t source_id) {
  ++refused_;
  if (source_id >= source_refusals_.size()) {
    source_refusals_.resize(source_id + 1, 0);
  }
  ++source_refusals_[source_id];
}

void Metrics::record_completion(size_t /*request_id*/, size_t source_id,
                                double time_in_system, double waiting_time,
                                double service_time) {
  ++completed_;
  sum_time_in_system_ += time_in_system;
  sum_waiting_time_ += waiting_time;
  sum_service_time_ += service_time;

  // Per-source tracking
  if (source_id >= source_sum_time_in_system_.size()) {
    source_sum_time_in_system_.resize(source_id + 1, 0.0);
    source_sum_waiting_time_.resize(source_id + 1, 0.0);
    source_sum_service_time_.resize(source_id + 1, 0.0);
    source_sum_sq_waiting_time_.resize(source_id + 1, 0.0);
    source_sum_sq_service_time_.resize(source_id + 1, 0.0);
    source_completions_.resize(source_id + 1, 0);
  }
  source_sum_time_in_system_[source_id] += time_in_system;
  source_sum_waiting_time_[source_id] += waiting_time;
  source_sum_service_time_[source_id] += service_time;
  source_sum_sq_waiting_time_[source_id] += waiting_time * waiting_time;
  source_sum_sq_service_time_[source_id] += service_time * service_time;
  ++source_completions_[source_id];
}

void Metrics::record_device_busy_time(size_t device_id, double busy_time) {
  if (device_id >= device_busy_times_.size()) {
    device_busy_times_.resize(device_id + 1, 0.0);
  }
  device_busy_times_[device_id] += busy_time;
}

double Metrics::get_refusal_probability() const {
  if (arrived_ == 0) return 0.0;
  return static_cast<double>(refused_) / arrived_;
}

double Metrics::get_avg_time_in_system() const {
  if (completed_ == 0) return 0.0;
  return sum_time_in_system_ / completed_;
}

double Metrics::get_avg_waiting_time() const {
  if (completed_ == 0) return 0.0;
  return sum_waiting_time_ / completed_;
}

double Metrics::get_avg_service_time() const {
  if (completed_ == 0) return 0.0;
  return sum_service_time_ / completed_;
}

double Metrics::get_device_utilization(size_t device_id,
                                       double total_time) const {
  if (total_time <= 0.0 || device_id >= device_busy_times_.size()) return 0.0;
  return device_busy_times_[device_id] / total_time;
}

size_t Metrics::get_arrived() const { return arrived_; }

size_t Metrics::get_refused() const { return refused_; }

size_t Metrics::get_completed() const { return completed_; }

void Metrics::reset() {
  arrived_ = 0;
  refused_ = 0;
  completed_ = 0;
  sum_time_in_system_ = 0.0;
  sum_waiting_time_ = 0.0;
  sum_service_time_ = 0.0;
  device_busy_times_.clear();
  source_arrivals_.clear();
  source_refusals_.clear();
  timeline_events_.clear();
  source_sum_time_in_system_.clear();
  source_sum_waiting_time_.clear();
  source_sum_service_time_.clear();
  source_sum_sq_waiting_time_.clear();
  source_sum_sq_service_time_.clear();
  source_completions_.clear();
}

size_t Metrics::get_source_arrivals(size_t source_id) const {
  if (source_id >= source_arrivals_.size()) return 0;
  return source_arrivals_[source_id];
}

double Metrics::get_source_refusal_probability(size_t source_id) const {
  size_t arrivals = get_source_arrivals(source_id);
  if (arrivals == 0) return 0.0;
  size_t refusals = (source_id < source_refusals_.size()) ? source_refusals_[source_id] : 0;
  return static_cast<double>(refusals) / arrivals;
}

double Metrics::get_source_avg_time_in_system(size_t source_id) const {
  size_t completions = get_source_completion_count(source_id);
  if (completions == 0) {
    return 0.0;
  }
  return source_sum_time_in_system_[source_id] / completions;
}

double Metrics::get_source_avg_waiting_time(size_t source_id) const {
  size_t completions = get_source_completion_count(source_id);
  if (completions == 0) {
    return 0.0;
  }
  return source_sum_waiting_time_[source_id] / completions;
}

double Metrics::get_source_avg_service_time(size_t source_id) const {
  size_t completions = get_source_completion_count(source_id);
  if (completions == 0) {
    return 0.0;
  }
  return source_sum_service_time_[source_id] / completions;
}

double Metrics::get_source_variance_waiting_time(size_t source_id) const {
  size_t completions = get_source_completion_count(source_id);
  if (completions == 0) {
    return 0.0;
  }
  double mean = source_sum_waiting_time_[source_id] / completions;
  double mean_sq = source_sum_sq_waiting_time_[source_id] / completions;
  return mean_sq - mean * mean;
}

double Metrics::get_source_variance_service_time(size_t source_id) const {
  size_t completions = get_source_completion_count(source_id);
  if (completions == 0) {
    return 0.0;
  }
  double mean = source_sum_service_time_[source_id] / completions;
  double mean_sq = source_sum_sq_service_time_[source_id] / completions;
  return mean_sq - mean * mean;
}

void Metrics::record_timeline_event(const TimelineEvent& event) {
  timeline_events_.push_back(event);
}

const std::vector<TimelineEvent>& Metrics::get_timeline_events() const {
  return timeline_events_;
}

void Metrics::clear_timeline_events() { timeline_events_.clear(); }

// Timeline event helper methods
void Metrics::record_arrival_event(double time, size_t request_id, size_t source_id) {
  record_timeline_event({time, TimelineEventType::ARRIVAL, request_id, source_id, 0, 0, ""});
}

void Metrics::record_service_start_event(double time, size_t request_id, size_t source_id,
                                         size_t device_id) {
  record_timeline_event({time, TimelineEventType::SERVICE_START, request_id, source_id,
                         device_id, 0, ""});
}

void Metrics::record_service_end_event(double time, size_t request_id, size_t source_id,
                                       size_t device_id) {
  record_timeline_event({time, TimelineEventType::SERVICE_END, request_id, source_id,
                         device_id, 0, ""});
}

void Metrics::record_buffer_place_event(double time, size_t request_id, size_t source_id,
                                        size_t buffer_slot) {
  record_timeline_event({time, TimelineEventType::BUFFER_PLACE, request_id, source_id, 0,
                         buffer_slot, ""});
}

void Metrics::record_buffer_take_event(double time, size_t request_id, size_t source_id,
                                       size_t device_id, size_t buffer_slot) {
  record_timeline_event({time, TimelineEventType::BUFFER_TAKE, request_id, source_id,
                         device_id, buffer_slot, ""});
}

void Metrics::record_buffer_displaced_event(double time, size_t request_id, size_t source_id) {
  record_timeline_event({time, TimelineEventType::BUFFER_DISPLACED, request_id, source_id,
                         0, 0, ""});
}

void Metrics::record_refusal_event(double time, size_t request_id, size_t source_id) {
  record_timeline_event({time, TimelineEventType::REFUSAL, request_id, source_id, 0, 0, ""});
}

size_t Metrics::get_source_completion_count(size_t source_id) const {
  if (source_id >= source_completions_.size()) {
    return 0;
  }
  return source_completions_[source_id];
}