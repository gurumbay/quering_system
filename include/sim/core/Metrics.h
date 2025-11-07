#ifndef SIM_CORE_METRICS_H_
#define SIM_CORE_METRICS_H_

#include <cstddef>
#include <string>
#include <vector>

namespace TimelineEventType {
constexpr const char* ARRIVAL = "arrival";
constexpr const char* SERVICE_START = "service_start";
constexpr const char* SERVICE_END = "service_end";
constexpr const char* REFUSAL = "refusal";
constexpr const char* BUFFER_PLACE = "buffer_place";
constexpr const char* BUFFER_TAKE = "buffer_take";
constexpr const char* BUFFER_DISPLACED = "buffer_displaced";
}  // namespace TimelineEventType

struct TimelineEvent {
  double time;
  std::string type;  // TimelineEventType constants
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
  void record_completion(size_t request_id, size_t source_id,
                         double time_in_system, double waiting_time,
                         double service_time);
  void record_device_busy_time(size_t device_id, double busy_time);

  // Timeline methods
  void record_timeline_event(const TimelineEvent& event);
  const std::vector<TimelineEvent>& get_timeline_events() const;
  void clear_timeline_events();

  // Timeline event helper methods (encapsulate TimelineEvent construction)
  void record_arrival_event(double time, size_t request_id, size_t source_id);
  void record_service_start_event(double time, size_t request_id, size_t source_id,
                                  size_t device_id);
  void record_service_end_event(double time, size_t request_id, size_t source_id,
                                size_t device_id);
  void record_buffer_place_event(double time, size_t request_id, size_t source_id,
                                 size_t buffer_slot);
  void record_buffer_take_event(double time, size_t request_id, size_t source_id,
                                size_t device_id, size_t buffer_slot);
  void record_buffer_displaced_event(double time, size_t request_id, size_t source_id);
  void record_refusal_event(double time, size_t request_id, size_t source_id);

  double get_refusal_probability() const;
  double get_avg_time_in_system() const;
  double get_avg_waiting_time() const;
  double get_avg_service_time() const;
  double get_device_utilization(size_t device_id, double total_time) const;
  size_t get_arrived() const;
  size_t get_refused() const;
  size_t get_completed() const;

  // Per-source statistics
  size_t get_source_arrivals(size_t source_id) const;
  double get_source_refusal_probability(size_t source_id) const;
  double get_source_avg_time_in_system(size_t source_id) const;
  double get_source_avg_waiting_time(size_t source_id) const;
  double get_source_avg_service_time(size_t source_id) const;
  double get_source_variance_waiting_time(size_t source_id) const;
  double get_source_variance_service_time(size_t source_id) const;

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

  // Per-source statistics
  std::vector<double> source_sum_time_in_system_;
  std::vector<double> source_sum_waiting_time_;
  std::vector<double> source_sum_service_time_;
  std::vector<double> source_sum_sq_waiting_time_;
  std::vector<double> source_sum_sq_service_time_;
  std::vector<size_t> source_completions_;

  // Helper methods
  size_t get_source_completion_count(size_t source_id) const;
};

#endif  // SIM_CORE_METRICS_H_
