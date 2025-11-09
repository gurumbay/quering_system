#include "sim/source/Source.h"

Source::Source(size_t id, std::unique_ptr<IDistribution> distribution)
    : id_(id),
      arrival_distribution_(std::move(distribution)),
      next_arrival_time_(NO_EVENT_TIME) {}

double Source::schedule_next_arrival(double current_time) {
  double interval = arrival_distribution_->generate();
  next_arrival_time_ = current_time + interval;
  return next_arrival_time_;
}

double Source::get_next_arrival_time() const {
  return next_arrival_time_;
}

bool Source::is_active() const {
  return next_arrival_time_ != NO_EVENT_TIME;
}

size_t Source::get_id() const {
  return id_;
}

void Source::clear_next_arrival_time() {
  next_arrival_time_ = NO_EVENT_TIME;
}

