#ifndef SIM_CORE_SIMULATION_CONFIG_H_
#define SIM_CORE_SIMULATION_CONFIG_H_

#include <cstddef>
#include <cstdint>
#include <vector>

struct SourceConfig {
  size_t id;
  double arrival_interval;  // 1/λ - interval between requests (time units)
  // λ = 1/arrival_interval - source intensity (requests per time unit)
};

struct SimulationConfig {
  size_t num_devices = 2;
  size_t buffer_capacity = 8;
  double device_intensity =
      1.5;  // μ - device intensity (requests per time unit)
  size_t max_arrivals = 10000;
  double max_time = 1e9;
  uint32_t seed = 42;
  std::vector<SourceConfig> sources;

  // Validate configuration
  bool validate() const;
};

#endif  // SIM_CORE_SIMULATION_CONFIG_H_

