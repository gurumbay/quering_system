#ifndef SIM_CORE_SIMULATOR_H_
#define SIM_CORE_SIMULATOR_H_

#include <cstddef>
#include <random>
#include <vector>

#include "sim/core/Buffer.h"
#include "sim/core/Device.h"
#include "sim/core/EventCalendar.h"
#include "sim/core/Metrics.h"
#include "sim/core/Request.h"

struct SourceConfig {
  size_t id;
  double arrival_interval;  // 1/λ - интервал между заявками (time units)
  // λ = 1/arrival_interval - интенсивность источника (requests per time unit)
};

struct SimulationConfig {
  size_t num_devices = 2;
  size_t buffer_capacity = 8;
  double device_intensity =
      1.5;  // μ - интенсивность приборов (requests per time unit)
  size_t max_arrivals = 10000;
  double max_time = 1e9;
  uint32_t seed = 42;
  std::vector<SourceConfig> sources;
};

class Simulator {
 public:
  explicit Simulator(const SimulationConfig& config);
  void run();
  void step();
  void handle_arrival(size_t source_id);
  void handle_service_end(size_t device_id);
  void schedule_next_arrival();
  void schedule_service_end(size_t device_id, size_t request_id,
                            double end_time);
  Metrics get_metrics() const;
  double get_current_time() const;

  // Debug methods
  void print_state() const;
  bool is_finished() const;

 private:
  std::vector<Device> devices_;
  Buffer buffer_;
  EventCalendar calendar_;
  Metrics metrics_;
  std::mt19937 rng_;
  std::exponential_distribution<double> service_dist_;
  std::vector<Request> requests_;
  double current_time_;
  size_t next_request_id_;
  size_t next_device_idx_;
  SimulationConfig config_;
  std::vector<size_t> source_arrivals_count_;
  bool initialized_;
};

#endif  // SIM_CORE_SIMULATOR_H_
