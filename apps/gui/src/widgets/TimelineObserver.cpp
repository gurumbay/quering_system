#include "TimelineObserver.h"

void TimelineObserver::on_arrival(const ArrivalEvent& event) {
  events_.push_back({event.time, "arrival", event.request_id, event.source_id, 0, 0});
}

void TimelineObserver::on_service_start(const ServiceStartEvent& event) {
  events_.push_back({event.time, "service_start", event.request_id, event.source_id,
                     event.device_id, 0});
}

void TimelineObserver::on_service_end(const ServiceEndEvent& event) {
  events_.push_back({event.time, "service_end", event.request_id, event.source_id,
                     event.device_id, 0});
}

void TimelineObserver::on_buffer_place(const BufferPlaceEvent& event) {
  events_.push_back({event.time, "buffer_place", event.request_id, event.source_id, 0,
                     event.buffer_slot});
}

void TimelineObserver::on_buffer_take(const BufferTakeEvent& event) {
  events_.push_back({event.time, "buffer_take", event.request_id, event.source_id,
                     event.device_id, event.buffer_slot});
}

void TimelineObserver::on_buffer_displaced(const BufferDisplacedEvent& event) {
  // Record both buffer_displaced (for buffer tracking) and refusal (for timeline display)
  events_.push_back({event.time, "buffer_displaced", event.request_id, event.source_id, 0, 0});
  events_.push_back({event.time, "refusal", event.request_id, event.source_id, 0, 0});
}

void TimelineObserver::on_refusal(const RefusalEvent& event) {
  events_.push_back({event.time, "refusal", event.request_id, event.source_id, 0, 0});
}
