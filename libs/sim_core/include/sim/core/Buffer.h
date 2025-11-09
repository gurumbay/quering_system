#ifndef SIM_CORE_BUFFER_H_
#define SIM_CORE_BUFFER_H_

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

#include "sim/core/Request.h"

class Buffer {
 public:
  Buffer(size_t capacity);
  std::optional<size_t> place_request(
      std::shared_ptr<Request> request);  // returns slot index
  std::shared_ptr<Request> displace_request();
  std::pair<std::shared_ptr<Request>, size_t>
  take_request();  // returns (request, slot_index)
  bool is_empty() const;
  bool is_full() const;
  size_t get_size() const;
  size_t get_capacity() const;

 private:
  std::vector<std::shared_ptr<Request>> slots_;
  size_t capacity_;
  size_t size_;
  size_t place_start_;
  size_t select_start_;
};

#endif  // SIM_CORE_BUFFER_H_
