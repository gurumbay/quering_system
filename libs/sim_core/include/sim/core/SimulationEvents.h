#ifndef SIM_CORE_SIMULATION_EVENTS_H_
#define SIM_CORE_SIMULATION_EVENTS_H_

#include <cstddef>

struct ArrivalEvent {
  size_t request_id;
  size_t source_id;
  double time;
};

struct ServiceStartEvent {
  size_t request_id;
  size_t source_id;
  size_t device_id;
  double time;
};

struct ServiceEndEvent {
  size_t request_id;
  size_t source_id;
  size_t device_id;
  double time;
  double time_in_system;
  double waiting_time;
  double service_time;
};

struct BufferPlaceEvent {
  size_t request_id;
  size_t source_id;
  size_t buffer_slot;
  double time;
};

struct BufferTakeEvent {
  size_t request_id;
  size_t source_id;
  size_t device_id;
  size_t buffer_slot;
  double time;
};

struct BufferDisplacedEvent {
  size_t request_id;
  size_t source_id;
  double time;
};

struct RefusalEvent {
  size_t request_id;
  size_t source_id;
  double time;
};

#endif  // SIM_CORE_SIMULATION_EVENTS_H_

