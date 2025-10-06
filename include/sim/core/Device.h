#ifndef SIM_CORE_DEVICE_H_
#define SIM_CORE_DEVICE_H_

#include <cstddef>

class Device {
public:
    Device(size_t id, bool is_busy = false);
    bool is_free() const;
    void start_service(size_t request_id, double now);

private:
    size_t id_;
    bool is_busy_;
    size_t current_request_id_;
    double busy_since_;
    // double total_busy_time_;
};

#endif // SIM_CORE_DEVICE_H_