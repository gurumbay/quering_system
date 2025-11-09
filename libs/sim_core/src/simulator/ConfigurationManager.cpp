#include "sim/simulator/ConfigurationManager.h"

#include "sim/device/DevicePool.h"
#include "sim/device/RoundRobinStrategy.h"
#include "sim/utils/ConstantDistribution.h"
#include "sim/utils/ExponentialDistribution.h"

bool ConfigurationManager::validate(const SimulationConfig& config) {
  // Check basic constraints
  if (config.num_devices == 0) return false;
  if (config.buffer_capacity == 0) return false;
  if (config.device_intensity <= 0.0) return false;
  if (config.max_arrivals == 0) return false;
  if (config.max_time <= 0.0) return false;
  if (config.sources.empty()) return false;

  // Check service distribution configuration
  if (config.service_distribution_type == ServiceDistributionType::Constant) {
    if (config.constant_service_time <= 0.0) return false;
  }

  // Check source configurations
  for (const auto& source : config.sources) {
    if (source.arrival_interval <= 0.05) return false;
  }

  return true;
}

std::unique_ptr<IDeviceSelectionStrategy>
ConfigurationManager::create_device_selection_strategy(
    const SimulationConfig& /*config*/) {
  // Currently only round-robin is supported
  // Can be extended to support other strategies based on config
  return std::make_unique<RoundRobinStrategy>();
}

std::unique_ptr<DevicePool> ConfigurationManager::create_device_pool(
    const SimulationConfig& config) {
  auto strategy = create_device_selection_strategy(config);
  return std::make_unique<DevicePool>(config.num_devices, std::move(strategy));
}

std::unique_ptr<IDistribution> ConfigurationManager::create_distribution(
    const SimulationConfig& config) {
  switch (config.service_distribution_type) {
    case ServiceDistributionType::Exponential:
      return std::make_unique<ExponentialDistribution>(config.device_intensity,
                                                       config.seed);
    case ServiceDistributionType::Constant:
      return std::make_unique<ConstantDistribution>(
          config.constant_service_time);
    default:
      // Default to exponential
      return std::make_unique<ExponentialDistribution>(config.device_intensity,
                                                       config.seed);
  }
}
