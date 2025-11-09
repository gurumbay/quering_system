#ifndef SIM_DEVICE_DEVICE_H_
#define SIM_DEVICE_DEVICE_H_

#include <cstddef>
#include <memory>

#include "sim/utils/IDistribution.h"

class Request;

class Device {
 public:
  static constexpr double NO_EVENT_TIME = -1.0;

  Device(size_t id, std::unique_ptr<IDistribution> distribution);

  void start_service(std::shared_ptr<Request> request, double now);
  std::shared_ptr<Request> finish_service();
  std::shared_ptr<Request> get_current_request() const;

  double schedule_next_service_end(double current_time);
  double get_next_service_end_time() const;
  void clear_next_service_end_time();
  bool is_free() const;
  size_t get_id() const;

 private:
  size_t id_;
  bool busy_;
  std::shared_ptr<Request> current_request_;
  std::unique_ptr<IDistribution> service_distribution_;
  double next_service_end_time_;
};

#endif  // SIM_DEVICE_DEVICE_H_
