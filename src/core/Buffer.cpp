#include "sim/core/Buffer.h"
#include <algorithm>

// NEED TO TEST PLACE/DISPLACE AND TAKE POLICY !!!!!

Buffer::Buffer(size_t capacity) 
    : capacity_(capacity), size_(0), place_start_(0), select_start_(0) {
    slots_.resize(capacity_);
    occupied_.resize(capacity_, false);
}

bool Buffer::place_request(size_t request_id) {
    if (is_full()) {
        return false;
    }
    
    // D10Z3: Find first free slot starting from place_start_
    for (size_t i = 0; i < capacity_; ++i) {
        size_t idx = (place_start_ + i) % capacity_;
        if (!occupied_[idx]) {
            slots_[idx] = request_id;
            occupied_[idx] = true;
            place_start_ = (idx + 1) % capacity_;  // Rotate start pointer
            ++size_;
            return true;
        }
    }
    
    return false;  // Should never reach here if !is_full()
}

size_t Buffer::displace_request() {
    // D10O4: Displace last arrived request (for overflow handling)
    // This is called by Simulator when buffer is full and new request arrives
    // We need to find the "last arrived" - in our case, we'll displace from select_start_
    // since that's where we would take from next
    
    if (is_empty()) {
        return 0;  // No request to displace
    }
    
    // Find first occupied slot from select_start_ (this represents "last arrived" in our model)
    for (size_t i = 0; i < capacity_; ++i) {
        size_t idx = (select_start_ + i) % capacity_;
        if (occupied_[idx]) {
            size_t displaced_id = slots_[idx];
            occupied_[idx] = false;
            slots_[idx] = 0;
            --size_;
            return displaced_id;
        }
    }
    
    return 0;  // Should never reach here
}


std::optional<size_t> Buffer::take_request() {
    if (is_empty()) {
        return std::nullopt;
    }
    
    // D2B3: Find first occupied slot starting from select_start_
    for (size_t i = 0; i < capacity_; ++i) {
        size_t idx = (select_start_ + i) % capacity_;
        if (occupied_[idx]) {
            size_t request_id = slots_[idx];
            occupied_[idx] = false;
            slots_[idx] = 0;  // Clear slot
            select_start_ = (idx + 1) % capacity_;  // Rotate start pointer
            --size_;
            return request_id;
        }
    }
    
    return std::nullopt;  // Should never reach here if !is_empty()
}

bool Buffer::is_empty() const {
    return size_ == 0;
}

bool Buffer::is_full() const {
    return size_ == capacity_;
}

size_t Buffer::get_size() const {
    return size_;
}

size_t Buffer::get_capacity() const {
    return capacity_;
}
