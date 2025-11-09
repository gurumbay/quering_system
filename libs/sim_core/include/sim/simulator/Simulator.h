#ifndef SIM_SIMULATOR_SIMULATOR_H_
#define SIM_SIMULATOR_SIMULATOR_H_

#include <cstddef>
#include <memory>
#include <vector>

#include "sim/queue/Buffer.h"
#include "sim/device/DevicePool.h"
#include "sim/event/Event.h"
#include "sim/event/EventCalendar.h"
#include "sim/event/EventDispatcher.h"
#include "sim/metrics/Metrics.h"
#include "sim/simulator/SimulationConfig.h"
#include "sim/source/SourceManager.h"
#include "sim/observers/ISimulationObserver.h"
#include "sim/utils/IDistribution.h"

class Simulator {
 public:
  explicit Simulator(const SimulationConfig& config);
  ~Simulator() = default;

  // Simulation control
  void run();
  void step();

  // Metrics and state queries
  Metrics get_metrics() const { return metrics_; }
  double get_current_time() const { return current_time_; }

  // Observer management
  void add_observer(std::unique_ptr<ISimulationObserver> observer);

  // State query methods for UI
  std::vector<bool> get_device_states() const;
  std::vector<double> get_source_next_event_times() const;
  std::vector<double> get_device_next_event_times() const;
  std::vector<bool> get_source_states() const;

  // Query methods for state inspection
  const Buffer& get_buffer() const { return buffer_; }
  const DevicePool& get_device_pool() const { return *device_pool_; }
  size_t get_calendar_size() const { return calendar_.get_size(); }

  bool is_finished() const;

 private:
  SimulationConfig config_;

  // Core components
  std::unique_ptr<DevicePool> device_pool_;
  Buffer buffer_;
  EventCalendar calendar_;
  Metrics metrics_;
  std::unique_ptr<IDistribution> service_distribution_;
  std::unique_ptr<SourceManager> source_manager_;
  std::unique_ptr<EventDispatcher> dispatcher_;

  // Simulation state
  double current_time_;

  // Observers
  std::vector<std::unique_ptr<ISimulationObserver>> observers_;

  // Helper methods
  bool process_next_event();
};

#endif  // SIM_SIMULATOR_SIMULATOR_H_
