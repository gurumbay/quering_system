#include "sim/core/RoundRobinStrategy.h"

#include "sim/core/Device.h"

Device* RoundRobinStrategy::find_free_device(
    const std::vector<std::unique_ptr<Device>>& devices) {
  if (devices.empty()) {
    return nullptr;
  }

  // Start search from next_index_
  for (size_t offset = 0; offset < devices.size(); ++offset) {
    size_t index = (next_index_ + offset) % devices.size();
    if (devices[index] && devices[index]->is_free()) {
      next_index_ = (index + 1) % devices.size();
      return devices[index].get();
    }
  }

  return nullptr;
}
