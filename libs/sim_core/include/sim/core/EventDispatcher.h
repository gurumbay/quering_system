#ifndef SIM_CORE_EVENT_DISPATCHER_H_
#define SIM_CORE_EVENT_DISPATCHER_H_

#include <cstddef>
#include <memory>
#include <vector>

#include "sim/core/Buffer.h"
#include "sim/core/DevicePool.h"
#include "sim/core/EventCalendar.h"
#include "sim/core/Metrics.h"
#include "sim/core/Request.h"
#include "sim/core/SimulationConfig.h"
#include "sim/core/SimulationEvents.h"
#include "sim/core/SourceManager.h"
#include "sim/observers/ISimulationObserver.h"
#include "sim/utils/IDistribution.h"

class Device;

class EventDispatcher {
 public:
  EventDispatcher(SourceManager& source_manager, DevicePool& device_pool,
                  Buffer& buffer, EventCalendar& calendar,
                  IDistribution& service_distribution, Metrics& metrics,
                  const SimulationConfig& config,
                  std::vector<std::unique_ptr<ISimulationObserver>>& observers);

  void handle_arrival(size_t source_id, double current_time);
  void handle_service_end(Device* device, double current_time);

  // Schedule service end event, returns the end time
  double schedule_service_end(Device* device, std::shared_ptr<Request> request,
                              double end_time);

 private:
  SourceManager& source_manager_;
  DevicePool& device_pool_;
  Buffer& buffer_;
  EventCalendar& calendar_;
  IDistribution& service_distribution_;
  Metrics& metrics_;
  const SimulationConfig& config_;
  std::vector<std::unique_ptr<ISimulationObserver>>& observers_;

  // Helper methods
  void notify_arrival(const ArrivalEvent& event);
  void notify_service_start(const ServiceStartEvent& event);
  void notify_service_end(const ServiceEndEvent& event);
  void notify_buffer_place(const BufferPlaceEvent& event);
  void notify_buffer_take(const BufferTakeEvent& event);
  void notify_buffer_displaced(const BufferDisplacedEvent& event);
  void notify_refusal(const RefusalEvent& event);

  void start_device_service(Device* device, std::shared_ptr<Request> request,
                            double current_time);
  void handle_buffer_placement(std::shared_ptr<Request> request,
                               size_t source_id, double current_time);
};

#endif  // SIM_CORE_EVENT_DISPATCHER_H_
