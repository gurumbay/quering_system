#ifndef SIM_SIMULATOR_CONFIGURATION_MANAGER_H_
#define SIM_SIMULATOR_CONFIGURATION_MANAGER_H_

#include <memory>

#include "sim/device/DevicePool.h"
#include "sim/device/IDeviceSelectionStrategy.h"
#include "sim/simulator/SimulationConfig.h"
#include "sim/source/SourcePool.h"
#include "sim/utils/IDistribution.h"

class ConfigurationManager {
 public:
  static bool validate(const SimulationConfig& config);

  // Create device selection strategy (currently only round-robin)
  static std::unique_ptr<IDeviceSelectionStrategy>
  create_device_selection_strategy(const SimulationConfig& config);

  static std::unique_ptr<DevicePool> create_device_pool(
      const SimulationConfig& config);

  static std::unique_ptr<SourcePool> create_source_pool(
      const SimulationConfig& config);

  static std::unique_ptr<IDistribution> create_distribution(
      DistributionType type, double param, uint32_t seed = 0);
};

#endif  // SIM_SIMULATOR_CONFIGURATION_MANAGER_H_
