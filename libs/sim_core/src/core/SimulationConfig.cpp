#include "sim/core/SimulationConfig.h"

bool SimulationConfig::validate() const {
  // Check basic constraints
  if (num_devices == 0) return false;
  if (buffer_capacity == 0) return false;
  if (device_intensity <= 0.0) return false;
  if (max_arrivals == 0) return false;
  if (max_time <= 0.0) return false;
  if (sources.empty()) return false;

  // Check source configurations
  for (const auto& source : sources) {
    if (source.arrival_interval <= 0.05) return false;
  }

  return true;
}
