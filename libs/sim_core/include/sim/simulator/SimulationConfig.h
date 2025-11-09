#ifndef SIM_SIMULATOR_SIMULATION_CONFIG_H_
#define SIM_SIMULATOR_SIMULATION_CONFIG_H_

#include <cstddef>
#include <cstdint>
#include <vector>

struct SourceConfig {
  size_t id;
  double arrival_interval;  // 1/λ - interval between requests (time units)
  // λ = 1/arrival_interval - source intensity (requests per time unit)
};

enum class ServiceDistributionType {
  Exponential,  // Exponential distribution
  Constant      // Constant intervals
};

struct SimulationConfig {
  size_t num_devices;
  size_t buffer_capacity;
  double device_intensity;  // requests per time unit (μ)
  ServiceDistributionType service_distribution_type =
      ServiceDistributionType::Exponential;
  double constant_service_time;  // Used when type is Constant
  size_t max_arrivals;
  double max_time = 1e9;
  uint32_t seed;
  std::vector<SourceConfig> sources;
};

#endif  // SIM_SIMULATOR_SIMULATION_CONFIG_H_
