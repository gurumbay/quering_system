#include "sim/observers/MetricsObserver.h"

#include "sim/core/Metrics.h"
#include "sim/core/SimulationEvents.h"

MetricsObserver::MetricsObserver(Metrics& metrics) : metrics_(metrics) {}

void MetricsObserver::on_arrival(const ArrivalEvent& event) {
  metrics_.record_arrival(event.source_id);
  metrics_.record_arrival_event(event.time, event.request_id, event.source_id);
}

void MetricsObserver::on_service_start(const ServiceStartEvent& event) {
  metrics_.record_service_start_event(event.time, event.request_id,
                                      event.source_id, event.device_id);
}

void MetricsObserver::on_service_end(const ServiceEndEvent& event) {
  metrics_.record_completion(event.request_id, event.source_id,
                             event.time_in_system, event.waiting_time,
                             event.service_time);
  metrics_.record_device_busy_time(event.device_id, event.service_time);
  metrics_.record_service_end_event(event.time, event.request_id,
                                    event.source_id, event.device_id);
}

void MetricsObserver::on_buffer_place(const BufferPlaceEvent& event) {
  metrics_.record_buffer_place_event(event.time, event.request_id,
                                     event.source_id, event.buffer_slot);
}

void MetricsObserver::on_buffer_take(const BufferTakeEvent& event) {
  metrics_.record_buffer_take_event(event.time, event.request_id,
                                    event.source_id, event.device_id,
                                    event.buffer_slot);
}

void MetricsObserver::on_buffer_displaced(const BufferDisplacedEvent& event) {
  metrics_.record_refusal(event.source_id);
  metrics_.record_buffer_displaced_event(event.time, event.request_id,
                                         event.source_id);
  metrics_.record_refusal_event(event.time, event.request_id, event.source_id);
}

void MetricsObserver::on_refusal(const RefusalEvent& event) {
  metrics_.record_refusal(event.source_id);
  metrics_.record_refusal_event(event.time, event.request_id, event.source_id);
}

