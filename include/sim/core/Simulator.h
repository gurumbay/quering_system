#ifndef SIM_CORE_SIMULATOR_H_
#define SIM_CORE_SIMULATOR_H_

#include <cstddef>
#include <optional>
#include <random>
#include <vector>

#include "sim/core/Buffer.h"
#include "sim/core/Device.h"
#include "sim/core/Event.h"
#include "sim/core/EventCalendar.h"
#include "sim/core/Metrics.h"
#include "sim/core/Request.h"

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
};

class Simulator {
 public:
  explicit Simulator(const SimulationConfig& config);
  void run();
  void step();
  void handle_arrival(size_t source_id);
  void handle_service_end(size_t device_id);
  void schedule_service_end(size_t device_id, size_t request_id,
                            double end_time);
  Metrics get_metrics() const;
  double get_current_time() const;
  double get_total_simulation_time() const;

  // State query methods for event calendar
  std::vector<bool> get_device_states() const;
  std::vector<double> get_source_next_event_times() const;
  std::vector<double> get_device_next_event_times() const;
  std::vector<bool> get_source_states() const;

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
  std::vector<double> source_next_event_times_;
  std::vector<double> device_next_event_times_;
  double end_time_;

  // Helper methods
  bool process_next_event();
  void clear_event_tracking(const Event& event);
  size_t create_request(size_t source_id);
  std::optional<size_t> find_free_device();
  void start_device_service(size_t device_id, size_t request_id);
  void handle_buffer_placement(size_t request_id, size_t source_id);
  void schedule_next_arrival_for_source(size_t source_id);
};

#endif  // SIM_CORE_SIMULATOR_H_
