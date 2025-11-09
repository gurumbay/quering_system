#ifndef SIM_CORE_I_DEVICE_SELECTION_STRATEGY_H_
#define SIM_CORE_I_DEVICE_SELECTION_STRATEGY_H_

#include <cstddef>
#include <memory>
#include <vector>

class Device;

class IDeviceSelectionStrategy {
 public:
  virtual ~IDeviceSelectionStrategy() = default;

  virtual Device* find_free_device(
      const std::vector<std::unique_ptr<Device>>& devices) = 0;

  virtual void reset() {}
};

#endif  // SIM_CORE_I_DEVICE_SELECTION_STRATEGY_H_
