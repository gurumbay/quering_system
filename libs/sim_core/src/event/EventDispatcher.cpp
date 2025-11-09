#include "sim/event/EventDispatcher.h"

#include "sim/device/Device.h"
#include "sim/event/Event.h"
#include "sim/model/Request.h"
#include "sim/source/Source.h"
#include "sim/simulator/SimulationConfig.h"

EventDispatcher::EventDispatcher(
    SourcePool& source_pool, DevicePool& device_pool, Buffer& buffer,
    EventCalendar& calendar, Metrics& metrics, const SimulationConfig& config,
    std::vector<std::unique_ptr<ISimulationObserver>>& observers)
    : source_pool_(source_pool),
      device_pool_(device_pool),
      buffer_(buffer),
      calendar_(calendar),
      metrics_(metrics),
      config_(config),
      observers_(observers) {}

void EventDispatcher::handle_arrival(size_t source_id, double current_time) {
  Source& source = source_pool_.get_source(source_id);
  if (metrics_.get_arrived() >= config_.max_arrivals) {
    source.clear_next_arrival_time();
    return;
  }

  auto request = std::make_shared<Request>(source_id, current_time);

  ArrivalEvent event{request->get_id(), source_id, current_time};
  notify_arrival(event);

  auto free_device = device_pool_.find_free_device();
  if (free_device != nullptr) {
    start_device_service(free_device, request, current_time);
  } else {
    handle_buffer_placement(request, source_id, current_time);
  }

  if (metrics_.get_arrived() < config_.max_arrivals) {
    double next_time = source.schedule_next_arrival(current_time);
    if (next_time != Source::NO_EVENT_TIME) {
      Event next_arrival(next_time, EventType::arrival,
                        std::weak_ptr<Request>(), nullptr, source_id);
      calendar_.schedule(next_arrival);
    }
  } else {
    source.clear_next_arrival_time();
  }
}

void EventDispatcher::handle_service_end(Device* device, double current_time) {
  if (!device) {
    return;
  }

  auto finished_request = device->finish_service();

  if (finished_request) {
    double time_in_system = current_time - finished_request->get_arrival_time();
    double waiting_time = finished_request->get_service_start_time() -
                          finished_request->get_arrival_time();
    double service_time =
        current_time - finished_request->get_service_start_time();

    ServiceEndEvent event{finished_request->get_id(),
                          finished_request->get_source_id(),
                          device->get_id(),
                          current_time,
                          time_in_system,
                          waiting_time,
                          service_time};
    notify_service_end(event);
  }
  
  device->clear_next_service_end_time();
  if (!buffer_.is_empty()) {
    auto [next_request, buffer_slot_index] = buffer_.take_request();
    if (next_request) {
      BufferTakeEvent event{next_request->get_id(),
                            next_request->get_source_id(), device->get_id(),
                            buffer_slot_index, current_time};
      notify_buffer_take(event);

      start_device_service(device, next_request, current_time);
    }
  }
}

void EventDispatcher::start_device_service(Device* device,
                                           std::shared_ptr<Request> request,
                                           double current_time) {
  if (!device || !request) {
    return;
  }

  device->start_service(request, current_time);
  
  double service_end_time = device->schedule_next_service_end(current_time);
  Event service_end_event(service_end_time, EventType::service_end,
                         std::weak_ptr<Request>(request), device);
  calendar_.schedule(service_end_event);

  ServiceStartEvent event{request->get_id(), request->get_source_id(),
                          device->get_id(), current_time};
  notify_service_start(event);
}

void EventDispatcher::handle_buffer_placement(std::shared_ptr<Request> request,
                                              size_t source_id,
                                              double current_time) {
  if (!request) {
    return;
  }

  auto buffer_slot = buffer_.place_request(request);
  if (buffer_slot.has_value()) {
    BufferPlaceEvent event{request->get_id(), source_id, *buffer_slot,
                           current_time};
    notify_buffer_place(event);
  } else {
    // Buffer full: displace last arrived request
    auto displaced_request = buffer_.displace_request();

    if (displaced_request) {
      BufferDisplacedEvent displaced_event{displaced_request->get_id(),
                                           displaced_request->get_source_id(),
                                           current_time};
      notify_buffer_displaced(displaced_event);
    }

    // Place the new request in the freed slot
    auto new_slot = buffer_.place_request(request);
    if (new_slot.has_value()) {
      BufferPlaceEvent event{request->get_id(), source_id, *new_slot,
                             current_time};
      notify_buffer_place(event);
    }
  }
}

void EventDispatcher::notify_arrival(const ArrivalEvent& event) {
  for (auto& observer : observers_) {
    observer->on_arrival(event);
  }
}

void EventDispatcher::notify_service_start(const ServiceStartEvent& event) {
  for (auto& observer : observers_) {
    observer->on_service_start(event);
  }
}

void EventDispatcher::notify_service_end(const ServiceEndEvent& event) {
  for (auto& observer : observers_) {
    observer->on_service_end(event);
  }
}

void EventDispatcher::notify_buffer_place(const BufferPlaceEvent& event) {
  for (auto& observer : observers_) {
    observer->on_buffer_place(event);
  }
}

void EventDispatcher::notify_buffer_take(const BufferTakeEvent& event) {
  for (auto& observer : observers_) {
    observer->on_buffer_take(event);
  }
}

void EventDispatcher::notify_buffer_displaced(
    const BufferDisplacedEvent& event) {
  for (auto& observer : observers_) {
    observer->on_buffer_displaced(event);
  }
}

void EventDispatcher::notify_refusal(const RefusalEvent& event) {
  for (auto& observer : observers_) {
    observer->on_refusal(event);
  }
}
