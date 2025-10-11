#ifndef SIM_CORE_METRICS_H_
#define SIM_CORE_METRICS_H_

#include <cstddef>
#include <string>
#include <vector>

struct TimelineEvent {
  double time;
  std::string type;  // "arrival", "service_start", "service_end", "refusal",
                     // "buffer_place", "buffer_take"
  size_t request_id;
  size_t source_id;
  size_t device_id;
  size_t buffer_slot;
  std::string description;
};

class Metrics {
 public:
  Metrics();

  void record_arrival(size_t source_id);
  void record_refusal(size_t source_id);
  void record_completion(size_t request_id, double time_in_system,
                         double waiting_time, double service_time);
  void record_device_busy_time(size_t device_id, double busy_time);

  // Timeline methods
  void record_timeline_event(const TimelineEvent& event);
  const std::vector<TimelineEvent>& get_timeline_events() const;
  void clear_timeline_events();

  double get_refusal_probability() const;
  double get_avg_time_in_system() const;
  double get_avg_waiting_time() const;
  double get_avg_service_time() const;
  double get_device_utilization(size_t device_id, double total_time) const;
  size_t get_arrived() const;
  size_t get_refused() const;
  size_t get_completed() const;

  void reset();

 private:
  size_t arrived_;
  size_t refused_;
  size_t completed_;
  double sum_time_in_system_;
  double sum_waiting_time_;
  double sum_service_time_;
  std::vector<double> device_busy_times_;
  std::vector<size_t> source_arrivals_;
  std::vector<size_t> source_refusals_;
  std::vector<TimelineEvent> timeline_events_;
};

#endif  // SIM_CORE_METRICS_H_
