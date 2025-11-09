#ifndef SIM_DEVICE_ROUND_ROBIN_STRATEGY_H_
#define SIM_DEVICE_ROUND_ROBIN_STRATEGY_H_

#include "sim/device/IDeviceSelectionStrategy.h"

class RoundRobinStrategy : public IDeviceSelectionStrategy {
 public:
  RoundRobinStrategy() : next_index_(0) {}

  Device* find_free_device(
      const std::vector<std::unique_ptr<Device>>& devices) override;

  void reset() override { next_index_ = 0; }

 private:
  size_t next_index_;
};

#endif  // SIM_DEVICE_ROUND_ROBIN_STRATEGY_H_
