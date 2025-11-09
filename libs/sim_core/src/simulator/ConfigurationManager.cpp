#include "sim/simulator/ConfigurationManager.h"

#include "sim/device/DevicePool.h"
#include "sim/device/RoundRobinStrategy.h"
#include "sim/source/Source.h"
#include "sim/source/SourcePool.h"
#include "sim/utils/ConstantDistribution.h"
#include "sim/utils/ExponentialDistribution.h"

bool ConfigurationManager::validate(const SimulationConfig& config) {
  if (config.buffer_capacity == 0) return false;
  if (config.max_arrivals == 0) return false;
  if (config.max_time <= 0.0) return false;
  if (config.sources.empty()) return false;
  if (config.devices.empty()) return false;

  for (const auto& source : config.sources) {
    if (source.arrival_parameter <= 0.0) return false;
  }

  for (const auto& device : config.devices) {
    if (device.service_parameter <= 0.0) return false;
  }

  return true;
}

std::unique_ptr<IDeviceSelectionStrategy>
ConfigurationManager::create_device_selection_strategy(
    const SimulationConfig& /*config*/) {
  return std::make_unique<RoundRobinStrategy>();
}

std::unique_ptr<DevicePool> ConfigurationManager::create_device_pool(
    const SimulationConfig& config) {
  auto strategy = create_device_selection_strategy(config);
  std::vector<std::unique_ptr<IDistribution>> distributions;
  
  for (size_t i = 0; i < config.devices.size(); ++i) {
    const auto& device_config = config.devices[i];
    
    auto distribution = create_distribution(
        device_config.service_distribution_type,
        device_config.service_parameter,
        config.seed + static_cast<uint32_t>(i));
    distributions.push_back(std::move(distribution));
  }
  
  return std::make_unique<DevicePool>(
      config.devices.size(), 
      std::move(strategy),
      std::move(distributions));
}

std::unique_ptr<SourcePool> ConfigurationManager::create_source_pool(
    const SimulationConfig& config) {
  auto pool = std::make_unique<SourcePool>();
  for (size_t i = 0; i < config.sources.size(); ++i) {
    const auto& source_config = config.sources[i];
    
    auto distribution = create_distribution(
        source_config.arrival_distribution_type,
        source_config.arrival_parameter,
        config.seed + static_cast<uint32_t>(i));
    
    auto source = std::make_unique<Source>(i, std::move(distribution));
    pool->add_source(std::move(source));
  }
  return pool;
}

std::unique_ptr<IDistribution> ConfigurationManager::create_distribution(
    DistributionType type, double param, uint32_t seed) {
  switch (type) {
    case DistributionType::Exponential:
      return std::make_unique<ExponentialDistribution>(param, seed);
    case DistributionType::Constant:
      return std::make_unique<ConstantDistribution>(param);
    default:
      // Default to exponential
      return std::make_unique<ExponentialDistribution>(param, seed);
  }
}
