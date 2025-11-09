#ifndef SIM_SOURCE_SOURCE_H_
#define SIM_SOURCE_SOURCE_H_

#include <cstddef>
#include <memory>

#include "sim/utils/IDistribution.h"

class Source {
 public:
  static constexpr double NO_EVENT_TIME = -1.0;

  Source(size_t id, std::unique_ptr<IDistribution> distribution);
  
  double schedule_next_arrival(double current_time);
  double get_next_arrival_time() const;
  void clear_next_arrival_time();
  bool is_active() const;
  size_t get_id() const;

 private:
  size_t id_;
  std::unique_ptr<IDistribution> arrival_distribution_;
  double next_arrival_time_;
};

#endif  // SIM_SOURCE_SOURCE_H_

