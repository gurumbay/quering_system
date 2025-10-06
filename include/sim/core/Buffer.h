#ifndef SIM_CORE_BUFFER_H_
#define SIM_CORE_BUFFER_H_

#include <optional>
#include <vector>
#include <cstddef>
#include "sim/core/Request.h"

class Buffer {
public:
    Buffer(size_t capacity);
    bool place_request(size_t request_id);
    size_t displace_request();
    std::optional<size_t> take_request();
    bool is_empty() const;
    bool is_full() const;
    size_t get_size() const;
    size_t get_capacity() const;

private:

    std::vector<size_t> slots_;
    std::vector<bool> occupied_;
    size_t capacity_;
    size_t size_;
    size_t place_start_;
    size_t select_start_;
};

#endif // SIM_CORE_BUFFER_H_
