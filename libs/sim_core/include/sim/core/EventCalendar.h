#ifndef SIM_CORE_EVENT_CALENDAR_H_
#define SIM_CORE_EVENT_CALENDAR_H_

#include <cstddef>
#include <queue>
#include <vector>

#include "sim/core/Event.h"

class EventCalendar {
 public:
  static constexpr double NO_EVENT_TIME = -1.0;

  EventCalendar();
  void schedule(const Event& event);
  Event pop_next();
  double get_next_time() const;
  size_t get_size() const;
  bool is_empty() const;

  // Query methods for source event times
  double get_next_time_for_source(size_t source_id) const;
  std::vector<double> get_all_source_next_times(size_t num_sources) const;
  bool has_event_for_source(size_t source_id) const;

  // Query methods for device event times
  double get_next_time_for_device(size_t device_id) const;
  std::vector<double> get_all_device_next_times(size_t num_devices) const;
  bool has_event_for_device(size_t device_id) const;

 private:
  std::priority_queue<Event> events_;
  std::vector<double> source_next_times_;
  std::vector<double> device_next_times_;
};

#endif  // SIM_CORE_EVENT_CALENDAR_H_
