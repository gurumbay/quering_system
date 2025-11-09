#include "sim/core/Simulator.h"

#include <stdexcept>

#include "sim/core/ConfigurationManager.h"
#include "sim/core/Event.h"
#include "sim/core/EventDispatcher.h"
#include "sim/observers/MetricsObserver.h"

Simulator::Simulator(const SimulationConfig& config)
    : config_(config), buffer_(config.buffer_capacity) {
  if (!ConfigurationManager::validate(config)) {
    throw std::invalid_argument("Invalid simulation configuration");
  }

  // Create core components
  device_pool_ = ConfigurationManager::create_device_pool(config);
  service_distribution_ = ConfigurationManager::create_distribution(config);
  source_manager_ = std::make_unique<SourceManager>(config, calendar_);

  dispatcher_ = std::make_unique<EventDispatcher>(
      *source_manager_, *device_pool_, buffer_, calendar_,
      *service_distribution_, metrics_, config_, observers_);

  // Initialize simulation state
  current_time_ = 0.0;

  // Create and add MetricsObserver
  auto metrics_observer = std::make_unique<MetricsObserver>(metrics_);
  observers_.push_back(std::move(metrics_observer));

  // Schedule initial arrivals for all sources
  source_manager_->schedule_initial_arrivals();
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

  switch (event.get_type()) {
    case EventType::arrival:
      dispatcher_->handle_arrival(event.get_source_id(), current_time_);
      break;
    case EventType::service_end: {
      Device* device = event.get_device();
      if (device) {
        dispatcher_->handle_service_end(device, current_time_);
      }
      break;
    }
  }

  return true;
}

void Simulator::run() {
  while (!is_finished()) {
    process_next_event();
  }
}

void Simulator::step() { process_next_event(); }

void Simulator::add_observer(std::unique_ptr<ISimulationObserver> observer) {
  observers_.push_back(std::move(observer));
}

std::vector<bool> Simulator::get_device_states() const {
  return device_pool_->get_device_states();
}

std::vector<double> Simulator::get_source_next_event_times() const {
  return source_manager_->get_all_next_event_times();
}

std::vector<double> Simulator::get_device_next_event_times() const {
  return calendar_.get_all_device_next_times(config_.num_devices);
}

std::vector<bool> Simulator::get_source_states() const {
  return source_manager_->get_all_source_states();
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
  const auto& devices = device_pool_->get_all_devices();
  for (const auto& device : devices) {
    if (device && !device->is_free()) {
      return false;  // Still have requests being serviced
    }
  }

  // All conditions met - simulation finished
  return true;
}
