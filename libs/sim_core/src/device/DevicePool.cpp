#include "sim/device/DevicePool.h"

#include <stdexcept>

DevicePool::DevicePool(size_t num_devices,
                       std::unique_ptr<IDeviceSelectionStrategy> strategy,
                       std::vector<std::unique_ptr<IDistribution>> distributions)
    : strategy_(std::move(strategy)) {
  devices_.reserve(num_devices);
  for (size_t i = 0; i < num_devices; ++i) {
    auto distribution = std::move(distributions[i]);
    devices_.emplace_back(std::make_unique<Device>(i, std::move(distribution)));
  }
}

Device* DevicePool::find_free_device() {
  return strategy_->find_free_device(devices_);
}

Device& DevicePool::get_device(size_t id) {
  if (id >= devices_.size() || !devices_[id]) {
    throw std::out_of_range("Device ID out of range");
  }
  return *devices_[id];
}

const Device& DevicePool::get_device(size_t id) const {
  if (id >= devices_.size() || !devices_[id]) {
    throw std::out_of_range("Device ID out of range");
  }
  return *devices_[id];
}

std::vector<bool> DevicePool::get_device_states() const {
  std::vector<bool> states;
  states.reserve(devices_.size());
  for (const auto& device : devices_) {
    if (device) {
      states.push_back(!device->is_free());  // true = busy
    } else {
      states.push_back(false);
    }
  }
  return states;
}

std::vector<double> DevicePool::get_all_next_event_times() const {
  std::vector<double> times;
  times.reserve(devices_.size());
  for (const auto& device : devices_) {
    if (device) {
      times.push_back(device->get_next_service_end_time());
    } else {
      times.push_back(Device::NO_EVENT_TIME);
    }
  }
  return times;
}

const std::vector<std::unique_ptr<Device>>& DevicePool::get_all_devices() const {
  return devices_;
}

size_t DevicePool::size() const {
  return devices_.size();
}

void DevicePool::reset_strategy() {
  if (strategy_) {
    strategy_->reset();
  }
}
