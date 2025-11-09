#include "sim/core/EventCalendar.h"

#include <algorithm>

#include "sim/core/Event.h"

EventCalendar::EventCalendar() {}

void EventCalendar::schedule(const Event& event) {
  events_.push(event);

  // Update tracking vectors based on event type
  if (event.get_type() == EventType::arrival) {
    size_t source_id = event.get_source_id();
    double event_time = event.get_time();
    // Resize vector if needed
    if (source_id >= source_next_times_.size()) {
      source_next_times_.resize(source_id + 1, NO_EVENT_TIME);
    }
    // Keep the earliest event time for each source
    if (source_next_times_[source_id] == NO_EVENT_TIME ||
        event_time < source_next_times_[source_id]) {
      source_next_times_[source_id] = event_time;
    }
  } else if (event.get_type() == EventType::service_end) {
    size_t device_id = event.get_device_id();
    double event_time = event.get_time();
    // Resize vector if needed
    if (device_id >= device_next_times_.size()) {
      device_next_times_.resize(device_id + 1, NO_EVENT_TIME);
    }
    // Keep the earliest event time for each device
    if (device_next_times_[device_id] == NO_EVENT_TIME ||
        event_time < device_next_times_[device_id]) {
      device_next_times_[device_id] = event_time;
    }
  }
}

Event EventCalendar::pop_next() {
  Event event = events_.top();
  events_.pop();

  // Update tracking vectors when event is popped
  // Set to NO_EVENT_TIME since this event is being processed
  // The next event for this source/device will be added when scheduled
  if (event.get_type() == EventType::arrival) {
    size_t source_id = event.get_source_id();
    // If this was the tracked event, clear it
    if (source_id < source_next_times_.size() &&
        source_next_times_[source_id] == event.get_time()) {
      source_next_times_[source_id] = NO_EVENT_TIME;
    }
  } else if (event.get_type() == EventType::service_end) {
    size_t device_id = event.get_device_id();
    // If this was the tracked event, clear it
    if (device_id < device_next_times_.size() &&
        device_next_times_[device_id] == event.get_time()) {
      device_next_times_[device_id] = NO_EVENT_TIME;
    }
  }

  return event;
}

double EventCalendar::get_next_time() const {
  if (events_.empty()) {
    return NO_EVENT_TIME;
  }
  return events_.top().get_time();
}

size_t EventCalendar::get_size() const { return events_.size(); }

bool EventCalendar::is_empty() const { return events_.empty(); }

double EventCalendar::get_next_time_for_source(size_t source_id) const {
  if (source_id >= source_next_times_.size()) {
    return NO_EVENT_TIME;
  }
  return source_next_times_[source_id];
}

std::vector<double> EventCalendar::get_all_source_next_times(
    size_t num_sources) const {
  std::vector<double> result(num_sources, NO_EVENT_TIME);
  size_t copy_size = std::min(num_sources, source_next_times_.size());
  for (size_t i = 0; i < copy_size; ++i) {
    result[i] = source_next_times_[i];
  }
  return result;
}

bool EventCalendar::has_event_for_source(size_t source_id) const {
  if (source_id >= source_next_times_.size()) {
    return false;
  }
  return source_next_times_[source_id] != NO_EVENT_TIME;
}

double EventCalendar::get_next_time_for_device(size_t device_id) const {
  if (device_id >= device_next_times_.size()) {
    return NO_EVENT_TIME;
  }
  return device_next_times_[device_id];
}

std::vector<double> EventCalendar::get_all_device_next_times(
    size_t num_devices) const {
  std::vector<double> result(num_devices, NO_EVENT_TIME);
  size_t copy_size = std::min(num_devices, device_next_times_.size());
  for (size_t i = 0; i < copy_size; ++i) {
    result[i] = device_next_times_[i];
  }
  return result;
}

bool EventCalendar::has_event_for_device(size_t device_id) const {
  if (device_id >= device_next_times_.size()) {
    return false;
  }
  return device_next_times_[device_id] != NO_EVENT_TIME;
}
