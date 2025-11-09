#ifndef SIM_OBSERVERS_I_SIMULATION_OBSERVER_H_
#define SIM_OBSERVERS_I_SIMULATION_OBSERVER_H_

#include "sim/event/SimulationEvents.h"

class ISimulationObserver {
 public:
  virtual ~ISimulationObserver() = default;
  virtual void on_arrival(const ArrivalEvent&) {}
  virtual void on_service_start(const ServiceStartEvent&) {}
  virtual void on_service_end(const ServiceEndEvent&) {}
  virtual void on_buffer_place(const BufferPlaceEvent&) {}
  virtual void on_buffer_take(const BufferTakeEvent&) {}
  virtual void on_buffer_displaced(const BufferDisplacedEvent&) {}
  virtual void on_refusal(const RefusalEvent&) {}
};

#endif  // SIM_OBSERVERS_I_SIMULATION_OBSERVER_H_

