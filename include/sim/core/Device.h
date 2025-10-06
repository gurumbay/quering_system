#ifndef SIM_CORE_DEVICE_H_
#define SIM_CORE_DEVICE_H_

#include <cstddef>

class Device {
public:
    Device(size_t id);
    bool is_free() const;
    void start_service(size_t request_id, double now);
    size_t finish_service(double now);
    double get_utilization(double total_time) const;
    size_t get_id() const;
    size_t get_current_request_id() const;

private:
    size_t id_;
    bool busy_;
    size_t current_request_id_;
    double busy_since_;
    double total_busy_time_;
};

#endif // SIM_CORE_DEVICE_H_