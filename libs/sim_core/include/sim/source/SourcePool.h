#ifndef SIM_SOURCE_SOURCE_POOL_H_
#define SIM_SOURCE_SOURCE_POOL_H_

#include <cstddef>
#include <memory>
#include <vector>

#include "sim/source/Source.h"

class SourcePool {
 public:
  SourcePool() = default;

  void add_source(std::unique_ptr<Source> source);
  Source& get_source(size_t id);
  const Source& get_source(size_t id) const;
  const std::vector<std::unique_ptr<Source>>& get_all_sources() const;
  std::vector<bool> get_source_states() const;
  std::vector<double> get_all_next_event_times() const;
  size_t size() const;

 private:
  std::vector<std::unique_ptr<Source>> sources_;
};

#endif  // SIM_SOURCE_SOURCE_POOL_H_

