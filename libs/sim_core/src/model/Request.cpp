#include "sim/model/Request.h"

size_t Request::next_id_ = 1;

Request::Request(size_t source_id, double t_arrival)
    : id_(next_id_++),
      source_id_(source_id),
      t_arrival_(t_arrival),
      t_service_start_(0.0) {}

size_t Request::get_id() const { return id_; }

size_t Request::get_source_id() const { return source_id_; }

double Request::get_arrival_time() const { return t_arrival_; }

void Request::set_service_start_time(double time) { t_service_start_ = time; }

double Request::get_service_start_time() const { return t_service_start_; }
