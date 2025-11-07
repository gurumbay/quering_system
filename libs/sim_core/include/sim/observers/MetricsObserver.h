#ifndef SIM_OBSERVERS_METRICS_OBSERVER_H_
#define SIM_OBSERVERS_METRICS_OBSERVER_H_

#include "sim/observers/ISimulationObserver.h"

class Metrics;

class MetricsObserver : public ISimulationObserver {
 public:
  explicit MetricsObserver(Metrics& metrics);
  ~MetricsObserver() override = default;

  void on_arrival(const ArrivalEvent& event) override;
  void on_service_start(const ServiceStartEvent& event) override;
  void on_service_end(const ServiceEndEvent& event) override;
  void on_buffer_place(const BufferPlaceEvent& event) override;
  void on_buffer_take(const BufferTakeEvent& event) override;
  void on_buffer_displaced(const BufferDisplacedEvent& event) override;
  void on_refusal(const RefusalEvent& event) override;

 private:
  Metrics& metrics_;
};

#endif  // SIM_OBSERVERS_METRICS_OBSERVER_H_

