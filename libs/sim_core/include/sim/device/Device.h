#ifndef SIM_DEVICE_DEVICE_H_
#define SIM_DEVICE_DEVICE_H_

#include <cstddef>
#include <memory>

class Request;

class Device {
 public:
  Device(size_t id);
  bool is_free() const;
  void start_service(std::shared_ptr<Request> request, double now);
  std::shared_ptr<Request> finish_service();
  size_t get_id() const;
  std::shared_ptr<Request> get_current_request() const;

 private:
  size_t id_;
  bool busy_;
  std::shared_ptr<Request> current_request_;
};

#endif  // SIM_DEVICE_DEVICE_H_
