#include "sim/core/Buffer.h"

#include <algorithm>

// NEED TO TEST PLACE/DISPLACE AND TAKE POLICY !!!!!

Buffer::Buffer(size_t capacity)
    : capacity_(capacity), size_(0), place_start_(0), select_start_(0) {
  slots_.resize(capacity_);
  occupied_.resize(capacity_, false);
}

std::optional<size_t> Buffer::place_request(size_t request_id) {
  if (is_full()) {
    return std::nullopt;
  }

  // D10Z3: Find first free slot starting from place_start_
  for (size_t idx = 0; idx < capacity_; ++idx) {
    if (!occupied_[idx]) {
      slots_[idx] = request_id;
      occupied_[idx] = true;
      ++size_;
      return idx;  // Return slot index
    }
  }

  return std::nullopt;  // Should never reach here if !is_full()
}

size_t Buffer::displace_request() {
  // D10O4: Displace last arrived request (for overflow handling)
  // Last arrived request is the one that was placed just before place_start_
  // i.e., at position (place_start_ - 1 + capacity_) % capacity_

  if (is_empty()) {
    return 0;  // No request to displace
  }

  // Last arrived request is just before place_start_ pointer
  size_t last_idx = (place_start_ - 1 + capacity_) % capacity_;

  // Search backwards from last placed to find the actual last occupied slot
  for (size_t i = 0; i < capacity_; ++i) {
    size_t idx = (last_idx - i + capacity_) % capacity_;
    if (occupied_[idx]) {
      size_t displaced_id = slots_[idx];
      occupied_[idx] = false;
      slots_[idx] = 0;
      --size_;
      // Update place_start_ to point to the freed slot so next placement starts
      // from here
      place_start_ = idx;
      return displaced_id;
    }
  }

  return 0;  // Should never reach here
}

std::pair<std::optional<size_t>, size_t> Buffer::take_request() {
  if (is_empty()) {
    return {std::nullopt, 0};
  }

  // D2B3: Find first occupied slot starting from select_start_
  for (size_t i = 0; i < capacity_; ++i) {
    size_t idx = (select_start_ + i) % capacity_;
    if (occupied_[idx]) {
      size_t request_id = slots_[idx];
      occupied_[idx] = false;
      slots_[idx] = 0;                        // Clear slot
      select_start_ = (idx + 1) % capacity_;  // Rotate start pointer
      --size_;
      return {request_id, idx};  // Return request_id and slot index
    }
  }

  return {std::nullopt, 0};  // Should never reach here if !is_empty()
}

bool Buffer::is_empty() const { return size_ == 0; }

bool Buffer::is_full() const { return size_ == capacity_; }

size_t Buffer::get_size() const { return size_; }

size_t Buffer::get_capacity() const { return capacity_; }
