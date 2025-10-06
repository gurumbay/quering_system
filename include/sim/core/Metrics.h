#ifndef SIM_CORE_METRICS_H_
#define SIM_CORE_METRICS_H_

#include <cstddef>
#include <vector>

class Metrics {
public:
    Metrics();
    
    void record_arrival(size_t source_id);
    void record_refusal(size_t source_id);
    void record_completion(size_t request_id, double time_in_system, double waiting_time, double service_time);
    void record_device_busy_time(size_t device_id, double busy_time);
    
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
};

#endif // SIM_CORE_METRICS_H_
