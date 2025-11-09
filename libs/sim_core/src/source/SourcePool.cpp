#include "sim/source/SourcePool.h"

#include <stdexcept>

void SourcePool::add_source(std::unique_ptr<Source> source) {
  sources_.push_back(std::move(source));
}

Source& SourcePool::get_source(size_t id) {
  if (id >= sources_.size() || !sources_[id]) {
    throw std::out_of_range("Source ID out of range");
  }
  return *sources_[id];
}

const Source& SourcePool::get_source(size_t id) const {
  if (id >= sources_.size() || !sources_[id]) {
    throw std::out_of_range("Source ID out of range");
  }
  return *sources_[id];
}

const std::vector<std::unique_ptr<Source>>& SourcePool::get_all_sources() const {
  return sources_;
}

std::vector<bool> SourcePool::get_source_states() const {
  std::vector<bool> states;
  states.reserve(sources_.size());
  for (const auto& source : sources_) {
    if (source) {
      states.push_back(source->is_active());
    } else {
      states.push_back(false);
    }
  }
  return states;
}

std::vector<double> SourcePool::get_all_next_event_times() const {
  std::vector<double> times;
  times.reserve(sources_.size());
  for (const auto& source : sources_) {
    if (source) {
      times.push_back(source->get_next_arrival_time());
    } else {
      times.push_back(Source::NO_EVENT_TIME);
    }
  }
  return times;
}

size_t SourcePool::size() const {
  return sources_.size();
}

