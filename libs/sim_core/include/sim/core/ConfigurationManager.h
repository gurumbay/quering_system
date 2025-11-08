#ifndef SIM_CORE_CONFIGURATION_MANAGER_H_
#define SIM_CORE_CONFIGURATION_MANAGER_H_

#include <memory>

#include "sim/core/DevicePool.h"
#include "sim/core/IDeviceSelectionStrategy.h"
#include "sim/core/SimulationConfig.h"
#include "sim/utils/IDistribution.h"

class ConfigurationManager {
 public:
  static bool validate(const SimulationConfig& config);

  // Create device selection strategy (currently only round-robin)
  static std::unique_ptr<IDeviceSelectionStrategy>
  create_device_selection_strategy(const SimulationConfig& config);

  static std::unique_ptr<DevicePool> create_device_pool(
      const SimulationConfig& config);

  static std::unique_ptr<IDistribution> create_distribution(
      const SimulationConfig& config);
};

#endif  // SIM_CORE_CONFIGURATION_MANAGER_H_
