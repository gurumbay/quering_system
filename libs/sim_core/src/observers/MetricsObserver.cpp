#include "sim/observers/MetricsObserver.h"

#include "sim/metrics/Metrics.h"
#include "sim/event/SimulationEvents.h"

MetricsObserver::MetricsObserver(Metrics& metrics) : metrics_(metrics) {}

void MetricsObserver::on_arrival(const ArrivalEvent& event) {
  metrics_.record_arrival(event.source_id);
}

void MetricsObserver::on_service_end(const ServiceEndEvent& event) {
  metrics_.record_completion(event.request_id, event.source_id,
                             event.time_in_system, event.waiting_time,
                             event.service_time);
  metrics_.record_device_busy_time(event.device_id, event.service_time);
}

void MetricsObserver::on_buffer_displaced(const BufferDisplacedEvent& event) {
  metrics_.record_refusal(event.source_id);
}

void MetricsObserver::on_refusal(const RefusalEvent& event) {
  metrics_.record_refusal(event.source_id);
}
