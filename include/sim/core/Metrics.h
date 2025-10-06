#ifndef SIM_CORE_METRICS_H_
#define SIM_CORE_METRICS_H_

#include <cstddef>

/**
 * @brief Class to collect and store simulation metrics
 */
class Metrics {
public:
    Metrics();
    
    // Getters for various metrics
    double get_average_response_time() const;
    double get_average_waiting_time() const;
    double get_throughput() const;
    double get_utilization() const;
    size_t get_total_requests() const;
    size_t get_completed_requests() const;
    size_t get_dropped_requests() const;
    
    // Methods to update metrics
    void record_request_arrival();
    void record_request_completion(double response_time);
    void record_request_drop();
    void update_device_utilization(double busy_time);
    void set_simulation_time(double time);
    
    // Reset metrics
    void reset();

private:
    size_t total_requests_;
    size_t completed_requests_;
    size_t dropped_requests_;
    double total_response_time_;
    double total_waiting_time_;
    double total_busy_time_;
    double simulation_time_;
};

#endif // SIM_CORE_METRICS_H_
