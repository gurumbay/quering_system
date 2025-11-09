#ifndef SIM_CORE_DEVICE_POOL_H_
#define SIM_CORE_DEVICE_POOL_H_

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

#include "sim/core/Device.h"
#include "sim/core/IDeviceSelectionStrategy.h"

class DevicePool {
 public:
  DevicePool(size_t num_devices,
             std::unique_ptr<IDeviceSelectionStrategy> strategy);

  Device* find_free_device();
  Device& get_device(size_t id);
  const Device& get_device(size_t id) const;
  const std::vector<std::unique_ptr<Device>>& get_all_devices() const;
  std::vector<bool> get_device_states() const;
  size_t size() const;
  void reset_strategy();

 private:
  std::vector<std::unique_ptr<Device>> devices_;
  std::unique_ptr<IDeviceSelectionStrategy> strategy_;
};

#endif  // SIM_CORE_DEVICE_POOL_H_
