#ifndef SIM_SOURCE_SOURCE_MANAGER_H_
#define SIM_SOURCE_SOURCE_MANAGER_H_

#include <cstddef>
#include <vector>

#include "sim/event/Event.h"
#include "sim/event/EventCalendar.h"
#include "sim/simulator/SimulationConfig.h"

class SourceManager {
 public:
  explicit SourceManager(const SimulationConfig& config,
                         EventCalendar& calendar);

  void schedule_initial_arrivals();
  void schedule_next_arrival(size_t source_id, double current_time,
                             size_t arrived_count, size_t max_arrivals);
  bool is_source_active(size_t source_id) const;
  std::vector<bool> get_all_source_states() const;
  std::vector<double> get_all_next_event_times() const;
  bool can_generate_more(size_t source_id, size_t arrived_count,
                         size_t max_arrivals) const;

 private:
  const SimulationConfig& config_;
  EventCalendar& calendar_;

  double calculate_next_arrival_time(size_t source_id,
                                     double current_time) const;
};

#endif  // SIM_SOURCE_SOURCE_MANAGER_H_
