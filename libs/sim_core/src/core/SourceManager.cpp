
#include "sim/core/SourceManager.h"

#include <algorithm>

#include "sim/core/Request.h"

SourceManager::SourceManager(const SimulationConfig& config,
                             EventCalendar& calendar)
    : config_(config), calendar_(calendar) {}

void SourceManager::schedule_initial_arrivals() {
  for (size_t i = 0; i < config_.sources.size(); ++i) {
    double arrival_time = config_.sources[i].arrival_interval;
    // Request doesn't exist yet, will be created in handle_arrival
    Event arrival_event(arrival_time, EventType::arrival,
                        std::weak_ptr<Request>(), nullptr, i);
    calendar_.schedule(arrival_event);
  }
}

void SourceManager::schedule_next_arrival(size_t source_id, double current_time,
                                          size_t arrived_count,
                                          size_t max_arrivals) {
  if (!can_generate_more(source_id, arrived_count, max_arrivals)) {
    return;
  }

  double next_arrival_time =
      calculate_next_arrival_time(source_id, current_time);
  // Request doesn't exist yet, will be created in handle_arrival
  Event next_arrival(next_arrival_time, EventType::arrival,
                     std::weak_ptr<Request>(), nullptr, source_id);
  calendar_.schedule(next_arrival);
}

bool SourceManager::is_source_active(size_t source_id) const {
  return calendar_.has_event_for_source(source_id);
}

std::vector<bool> SourceManager::get_all_source_states() const {
  std::vector<bool> states;
  states.reserve(config_.sources.size());
  for (size_t i = 0; i < config_.sources.size(); ++i) {
    states.push_back(calendar_.has_event_for_source(i));
  }
  return states;
}

std::vector<double> SourceManager::get_all_next_event_times() const {
  return calendar_.get_all_source_next_times(config_.sources.size());
}

bool SourceManager::can_generate_more(size_t source_id, size_t arrived_count,
                                      size_t max_arrivals) const {
  return source_id < config_.sources.size() && arrived_count < max_arrivals;
}

double SourceManager::calculate_next_arrival_time(size_t source_id,
                                                  double current_time) const {
  if (source_id >= config_.sources.size()) {
    return EventCalendar::NO_EVENT_TIME;
  }
  return current_time + config_.sources[source_id].arrival_interval;
}
