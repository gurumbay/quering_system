#ifndef SIM_MODEL_REQUEST_H_
#define SIM_MODEL_REQUEST_H_

#include <cstddef>

class Request {
 public:
  Request(size_t source_id, double t_arrival);
  size_t get_id() const;
  size_t get_source_id() const;
  double get_arrival_time() const;
  void set_service_start_time(double time);
  double get_service_start_time() const;

 private:
  static size_t next_id_;
  size_t id_;
  size_t source_id_;
  double t_arrival_;
  double t_service_start_;
};

#endif  // SIM_MODEL_REQUEST_H_
