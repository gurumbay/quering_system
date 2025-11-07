#include "sim/core/Simulator.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>

#include "sim/core/SimulationConfig.h"
#include "sim/core/SimulationEvents.h"
#include "sim/observers/ISimulationObserver.h"
#include "sim/observers/MetricsObserver.h"

namespace {
constexpr double NO_EVENT_TIME = -1.0;
constexpr size_t INVALID_REQUEST_ID = 0;
}  // namespace

Simulator::Simulator(const SimulationConfig& config)
    : config_(config),
      buffer_(config.buffer_capacity),
      rng_(config.seed),
      service_dist_(config.device_intensity),
      current_time_(0.0),
      next_request_id_(0),
      next_device_idx_(0) {
  devices_.reserve(config.num_devices);
  for (size_t i = 0; i < config.num_devices; ++i) {
    devices_.emplace_back(i);
  }

  source_arrivals_count_.resize(config.sources.size(), 0);
  source_next_event_times_.resize(config.sources.size(), NO_EVENT_TIME);
  device_next_event_times_.resize(config.num_devices, NO_EVENT_TIME);
  end_time_ = 0.0;

  // Create and add MetricsObserver
  auto metrics_observer = std::make_unique<MetricsObserver>(metrics_);
  observers_.push_back(std::move(metrics_observer));

  // Schedule initial arrivals for all sources
  for (size_t i = 0; i < config.sources.size(); ++i) {
    double arrival_time = config.sources[i].arrival_interval;
    Event arrival_event(arrival_time, EventType::arrival, 0, 0, i);
    calendar_.schedule(arrival_event);
    source_next_event_times_[i] = arrival_time;
  }
}

bool Simulator::process_next_event() {
  if (calendar_.is_empty()) {
    return false;
  }

  Event event = calendar_.pop_next();
  current_time_ = event.get_time();

  if (current_time_ > config_.max_time) {
    return false;
  }

  clear_event_tracking(event);

  switch (event.get_type()) {
    case EventType::arrival:
      handle_arrival(event.get_source_id());
      break;
    case EventType::service_end:
      handle_service_end(event.get_device_id());
      break;
  }

  return true;
}

void Simulator::clear_event_tracking(const Event& event) {
  switch (event.get_type()) {
    case EventType::arrival:
      if (event.get_source_id() < source_next_event_times_.size()) {
        source_next_event_times_[event.get_source_id()] = NO_EVENT_TIME;
      }
      break;
    case EventType::service_end:
      if (event.get_device_id() < device_next_event_times_.size()) {
        device_next_event_times_[event.get_device_id()] = NO_EVENT_TIME;
      }
      break;
  }
}

void Simulator::run() {
  while (process_next_event()) {
    if (is_finished()) {
      break;
    }
  }
}

void Simulator::step() {
  process_next_event();
}

size_t Simulator::create_request(size_t source_id) {
  size_t request_id = next_request_id_++;
  if (requests_.size() <= request_id) {
    requests_.reserve(request_id + 1);
    while (requests_.size() <= request_id) {
      requests_.emplace_back(0, 0, 0.0);  // Placeholder, will be overwritten
    }
  }
  requests_[request_id] = Request(request_id, source_id, current_time_);
  return request_id;
}

std::optional<size_t> Simulator::find_free_device() {
  // Device selection: round-robin starting from next_device_idx_
  for (size_t search_offset = 0; search_offset < devices_.size(); ++search_offset) {
    size_t device_index = (next_device_idx_ + search_offset) % devices_.size();
    if (devices_[device_index].is_free()) {
      next_device_idx_ = (device_index + 1) % devices_.size();
      return device_index;
    }
  }
  return std::nullopt;
}

void Simulator::start_device_service(size_t device_id, size_t request_id) {
  devices_[device_id].start_service(request_id, current_time_);
  requests_[request_id].set_service_start_time(current_time_);

  double service_time = service_dist_(rng_);
  double service_end_time = current_time_ + service_time;
  schedule_service_end(device_id, request_id, service_end_time);

  ServiceStartEvent event{request_id, requests_[request_id].get_source_id(),
                          device_id, current_time_};
  notify_service_start(event);
}

void Simulator::handle_buffer_placement(size_t request_id, size_t source_id) {
  auto buffer_slot = buffer_.place_request(request_id);
  if (buffer_slot.has_value()) {
    BufferPlaceEvent event{request_id, source_id, *buffer_slot, current_time_};
    notify_buffer_place(event);
  } else {
    // Buffer full: displace last arrived request and place new request
    size_t displaced_id = buffer_.displace_request();

    if (displaced_id != INVALID_REQUEST_ID &&
        displaced_id < requests_.size()) {
      size_t displaced_source = requests_[displaced_id].get_source_id();
      BufferDisplacedEvent displaced_event{displaced_id, displaced_source,
                                           current_time_};
      notify_buffer_displaced(displaced_event);
    }

    // Place the new request in the freed slot
    auto new_slot = buffer_.place_request(request_id);
    if (new_slot.has_value()) {
      BufferPlaceEvent event{request_id, source_id, *new_slot, current_time_};
      notify_buffer_place(event);
    }
  }
}

void Simulator::schedule_next_arrival_for_source(size_t source_id) {
  if (metrics_.get_arrived() < config_.max_arrivals) {
    double next_arrival_time =
        current_time_ + config_.sources[source_id].arrival_interval;
    Event next_arrival(next_arrival_time, EventType::arrival, 0, 0, source_id);
    calendar_.schedule(next_arrival);
    source_next_event_times_[source_id] = next_arrival_time;
  } else {
    source_next_event_times_[source_id] = NO_EVENT_TIME;
  }
}

void Simulator::handle_arrival(size_t source_id) {
  if (metrics_.get_arrived() >= config_.max_arrivals) {
    return;
  }

  size_t request_id = create_request(source_id);
  source_arrivals_count_[source_id]++;

  ArrivalEvent event{request_id, source_id, current_time_};
  notify_arrival(event);

  auto free_device = find_free_device();
  if (free_device.has_value()) {
    start_device_service(*free_device, request_id);
  } else {
    handle_buffer_placement(request_id, source_id);
  }

  schedule_next_arrival_for_source(source_id);
}

void Simulator::handle_service_end(size_t device_id) {
  size_t finished_id = devices_[device_id].finish_service(current_time_);

  if (finished_id < requests_.size()) {
    const Request& req = requests_[finished_id];
    double time_in_system = current_time_ - req.get_arrival_time();
    double waiting_time = req.get_service_start_time() - req.get_arrival_time();
    double service_time = current_time_ - req.get_service_start_time();

    ServiceEndEvent event{finished_id, req.get_source_id(), device_id,
                         current_time_, time_in_system, waiting_time,
                         service_time};
    notify_service_end(event);

    end_time_ = current_time_;
  }

  // Check buffer for waiting requests: take first occupied slot
  if (!buffer_.is_empty()) {
    auto [next_request, buffer_slot_index] = buffer_.take_request();
    if (next_request.has_value()) {
      size_t request_id = *next_request;
      BufferTakeEvent event{request_id, requests_[request_id].get_source_id(),
                            device_id, buffer_slot_index, current_time_};
      notify_buffer_take(event);

      start_device_service(device_id, request_id);
    } else {
      device_next_event_times_[device_id] = NO_EVENT_TIME;
    }
  } else {
    device_next_event_times_[device_id] = NO_EVENT_TIME;
  }
}


void Simulator::schedule_service_end(size_t device_id, size_t request_id,
                                     double end_time) {
  Event service_end_event(end_time, EventType::service_end, request_id,
                          device_id);
  calendar_.schedule(service_end_event);
  device_next_event_times_[device_id] = end_time;
}

Metrics Simulator::get_metrics() const { return metrics_; }

double Simulator::get_current_time() const { return current_time_; }

double Simulator::get_total_simulation_time() const {
  // Return current_time_ if simulation is active (still has events or buffer not empty),
  // otherwise return end_time_ (time when last request completed)
  if (!is_finished()) {
    return current_time_;
  }
  // Simulation finished - return the time when last request completed
  return (end_time_ > 0.0) ? end_time_ : current_time_;
}

void Simulator::print_state() const {
  std::cout << "\n=== SIMULATION STATE ===" << std::endl;
  std::cout << "Time: " << current_time_ << std::endl;
  std::cout << "Calendar size: " << calendar_.get_size() << std::endl;
  std::cout << "Buffer: " << buffer_.get_size() << "/" << buffer_.get_capacity()
            << std::endl;

  std::cout << "Devices:" << std::endl;
  for (size_t i = 0; i < devices_.size(); ++i) {
    std::cout << "  Device " << i << ": "
              << (devices_[i].is_free() ? "FREE" : "BUSY");
    if (!devices_[i].is_free()) {
      std::cout << " (request " << devices_[i].get_current_request_id() << ")";
    }
    std::cout << std::endl;
  }

  std::cout << "Metrics:" << std::endl;
  std::cout << "  Arrived: " << metrics_.get_arrived() << std::endl;
  std::cout << "  Refused: " << metrics_.get_refused() << std::endl;
  std::cout << "  Completed: " << metrics_.get_completed() << std::endl;
  std::cout << "  P_ref: " << metrics_.get_refusal_probability() << std::endl;
  std::cout << "========================\n" << std::endl;
}

bool Simulator::is_finished() const {
  // Check if max time exceeded
  if (current_time_ > config_.max_time) {
    return true;
  }
  
  // Check if max arrivals reached
  bool max_arrivals_reached = metrics_.get_arrived() >= config_.max_arrivals;
  
  if (!max_arrivals_reached) {
    return false;  // Still generating arrivals
  }
  
  // Max arrivals reached - check if system is empty
  // System is finished when:
  // 1. Calendar is empty (no future events)
  // 2. Buffer is empty (no waiting requests)
  // 3. All devices are free (no requests being serviced)
  if (!calendar_.is_empty()) {
    return false;  // Still have events to process
  }
  
  if (!buffer_.is_empty()) {
    return false;  // Still have requests in buffer
  }
  
  // Check if all devices are free
  for (const auto& device : devices_) {
    if (!device.is_free()) {
      return false;  // Still have requests being serviced
    }
  }
  
  // All conditions met - simulation finished
  return true;
}

std::vector<bool> Simulator::get_device_states() const {
  std::vector<bool> states;
  states.reserve(devices_.size());
  for (const auto& device : devices_) {
    states.push_back(!device.is_free());  // active = busy
  }
  return states;
}

std::vector<double> Simulator::get_source_next_event_times() const {
  return source_next_event_times_;
}

std::vector<double> Simulator::get_device_next_event_times() const {
  return device_next_event_times_;
}

std::vector<bool> Simulator::get_source_states() const {
  std::vector<bool> states;
  states.reserve(source_next_event_times_.size());
    for (double next_time : source_next_event_times_) {
    states.push_back(next_time != NO_EVENT_TIME);  // active = has scheduled event
  }
  return states;
}

void Simulator::add_observer(std::unique_ptr<ISimulationObserver> observer) {
  observers_.push_back(std::move(observer));
}

void Simulator::notify_arrival(const ArrivalEvent& event) {
  for (auto& observer : observers_) {
    observer->on_arrival(event);
  }
}

void Simulator::notify_service_start(const ServiceStartEvent& event) {
  for (auto& observer : observers_) {
    observer->on_service_start(event);
  }
}

void Simulator::notify_service_end(const ServiceEndEvent& event) {
  for (auto& observer : observers_) {
    observer->on_service_end(event);
  }
}

void Simulator::notify_buffer_place(const BufferPlaceEvent& event) {
  for (auto& observer : observers_) {
    observer->on_buffer_place(event);
  }
}

void Simulator::notify_buffer_take(const BufferTakeEvent& event) {
  for (auto& observer : observers_) {
    observer->on_buffer_take(event);
  }
}

void Simulator::notify_buffer_displaced(const BufferDisplacedEvent& event) {
  for (auto& observer : observers_) {
    observer->on_buffer_displaced(event);
  }
}

void Simulator::notify_refusal(const RefusalEvent& event) {
  for (auto& observer : observers_) {
    observer->on_refusal(event);
  }
}
