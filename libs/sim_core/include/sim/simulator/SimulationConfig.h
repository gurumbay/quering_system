#ifndef SIM_SIMULATOR_SIMULATION_CONFIG_H_
#define SIM_SIMULATOR_SIMULATION_CONFIG_H_

#include <cstddef>
#include <cstdint>
#include <vector>

enum class DistributionType {
  Constant,     // Constant intervals
  Exponential   // Exponential distribution
};

struct SourceConfig {
  size_t id;
  double arrival_parameter;
  DistributionType arrival_distribution_type = DistributionType::Constant;
};

struct DeviceConfig {
  size_t id;
  double service_parameter;
  DistributionType service_distribution_type = DistributionType::Exponential;
};

struct SimulationConfig {
  size_t buffer_capacity;
  size_t max_arrivals;
  double max_time = 1e9;
  uint32_t seed;
  std::vector<SourceConfig> sources;
  std::vector<DeviceConfig> devices;
};

#endif  // SIM_SIMULATOR_SIMULATION_CONFIG_H_
