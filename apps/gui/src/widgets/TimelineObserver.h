#pragma once

#include <string>
#include <vector>

#include "sim/core/SimulationEvents.h"
#include "sim/observers/ISimulationObserver.h"

struct TimelineEvent {
  double time;
  std::string type;  // "arrival", "service_start", "service_end", "refusal", "buffer_place", "buffer_take", "buffer_displaced"
  size_t request_id;
  size_t source_id;
  size_t device_id;
  size_t buffer_slot;
};

class TimelineObserver : public ISimulationObserver {
 public:
  TimelineObserver() = default;

  void on_arrival(const ArrivalEvent& event) override;
  void on_service_start(const ServiceStartEvent& event) override;
  void on_service_end(const ServiceEndEvent& event) override;
  void on_buffer_place(const BufferPlaceEvent& event) override;
  void on_buffer_take(const BufferTakeEvent& event) override;
  void on_buffer_displaced(const BufferDisplacedEvent& event) override;
  void on_refusal(const RefusalEvent& event) override;

  const std::vector<TimelineEvent>& get_events() const { return events_; }
  void clear_events() { events_.clear(); }

 private:
  std::vector<TimelineEvent> events_;
};

