#ifndef SIM_CORE_SIMULATOR_H_
#define SIM_CORE_SIMULATOR_H_

#include <vector>
#include <random>
#include <cstddef>
#include "sim/core/Device.h"
#include "sim/core/Buffer.h"
#include "sim/core/EventCalendar.h"
#include "sim/core/Metrics.h"

class Simulator {
public:
    Simulator();
    void run();
    void step();
    void handle_arrival();
    void handle_service_end();
    void schedule_next_arrival();
    // void schedule_service_end(); // ???
    Metrics get_metrics() const;
    double get_current_time() const;

private:
    std::vector<Device> devices_;
    Buffer buffer_;
    EventCalendar event_calendar_;
    std::mt19937 rng_;
    double current_time_;
    size_t next_request_id_;
    size_t next_device_idx_;
    // SimulationConfig config_;    // TODO
};

#endif // SIM_CORE_SIMULATOR_H_
