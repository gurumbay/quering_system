#include "sim/queue/Buffer.h"

Buffer::Buffer(size_t capacity)
    : capacity_(capacity), size_(0), place_start_(0), select_start_(0) {
  slots_.resize(capacity_, nullptr);
}

std::optional<size_t> Buffer::place_request(std::shared_ptr<Request> request) {
  if (is_full() || !request) {
    return std::nullopt;
  }

  // Find first free slot starting from index 0
  for (size_t idx = 0; idx < capacity_; ++idx) {
    if (!slots_[idx]) {
      slots_[idx] = request;
      ++size_;
      place_start_ = idx;  // Track last placed position
      return idx;          // Return slot index
    }
  }

  return std::nullopt;  // Should never reach here if !is_full()
}

std::shared_ptr<Request> Buffer::displace_request() {
  if (is_empty()) {
    return nullptr;
  }

  // Start searching backwards from the last placed position
  // Wrap around if we reach the beginning
  for (size_t offset = 0; offset < capacity_; ++offset) {
    size_t idx = (place_start_ - offset + capacity_) % capacity_;
    if (slots_[idx]) {
      std::shared_ptr<Request> displaced_request = slots_[idx];
      slots_[idx] = nullptr;
      --size_;
      // Update place_start_ to point to the slot before the displaced one
      // This ensures next displacement continues from the correct position
      place_start_ = (idx - 1 + capacity_) % capacity_;
      return displaced_request;
    }
  }

  return nullptr;  // Should never reach here
}

std::pair<std::shared_ptr<Request>, size_t> Buffer::take_request() {
  if (is_empty()) {
    return {nullptr, 0};
  }

  // Take first occupied slot starting from select_start_
  // (round-robin selection)
  for (size_t i = 0; i < capacity_; ++i) {
    size_t idx = (select_start_ + i) % capacity_;
    if (slots_[idx]) {
      std::shared_ptr<Request> request = slots_[idx];
      slots_[idx] = nullptr;                  // Clear slot
      select_start_ = (idx + 1) % capacity_;  // Rotate start pointer
      --size_;

      // If we removed the request at place_start_, update place_start_
      // to point to the previous occupied slot 
      // (for correct displacement tracking)
      if (idx == place_start_ && size_ > 0) {
        // Find the last occupied slot before the removed one
        for (size_t j = 1; j < capacity_; ++j) {
          size_t prev_idx = (idx - j + capacity_) % capacity_;
          if (slots_[prev_idx]) {
            place_start_ = prev_idx;
            break;
          }
        }
      }

      return {request, idx};  // Return request and slot index
    }
  }

  return {nullptr, 0};  // Should never reach here
}

bool Buffer::is_empty() const { return size_ == 0; }

bool Buffer::is_full() const { return size_ == capacity_; }

size_t Buffer::get_size() const { return size_; }

size_t Buffer::get_capacity() const { return capacity_; }
