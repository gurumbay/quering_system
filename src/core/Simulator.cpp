#include <algorithm>
#include <iostream>
#include "sim/core/Simulator.h"

Simulator::Simulator(const SimulationConfig& config) 
    : config_(config), buffer_(config.buffer_capacity), rng_(config.seed), 
      service_dist_(config.device_intensity), current_time_(0.0), next_request_id_(0), next_device_idx_(0), initialized_(false) {
    devices_.reserve(config.num_devices);
    for (size_t i = 0; i < config.num_devices; ++i) {
        devices_.emplace_back(i);
    }
    
    source_arrivals_count_.resize(config.sources.size(), 0);
}

void Simulator::run() {
    if (!initialized_) {
        schedule_next_arrival();
        initialized_ = true;
    }
    
    while (!calendar_.is_empty()) {
        Event event = calendar_.pop_next();
        current_time_ = event.get_time();
        
        if (current_time_ > config_.max_time) break;
        
        switch (event.get_type()) {
            case EventType::arrival:
                handle_arrival(event.get_source_id());
                break;
            case EventType::service_end:
                handle_service_end(event.get_device_id());
                break;
        }
        
        if (metrics_.get_arrived() >= config_.max_arrivals && 
            calendar_.is_empty() && buffer_.is_empty()) {
            break;
        }
    }
}

void Simulator::step() {
    if (!initialized_) {
        schedule_next_arrival();
        initialized_ = true;
    }

    if (!calendar_.is_empty()) {
        Event event = calendar_.pop_next();
        current_time_ = event.get_time();
        
        switch (event.get_type()) {
            case EventType::arrival:
                handle_arrival(event.get_source_id());
                break;
            case EventType::service_end:
                handle_service_end(event.get_device_id());
                break;
        }
    }
}

void Simulator::handle_arrival(size_t source_id) {
    
    if (metrics_.get_arrived() < config_.max_arrivals) {
        size_t request_id = next_request_id_++;
        
        // Ensure vector is large enough and add request
        if (requests_.size() <= request_id) {
            requests_.reserve(request_id + 1);
            while (requests_.size() <= request_id) {
                requests_.emplace_back(0, 0, 0.0); // placeholder
            }
        }
        requests_[request_id] = Request(request_id, source_id, current_time_);
        metrics_.record_arrival(source_id);
        source_arrivals_count_[source_id]++;
        
        // D2P2: Find free device round-robin
        bool device_found = false;
        for (size_t i = 0; i < devices_.size(); ++i) {
            size_t idx = (next_device_idx_ + i) % devices_.size();
            if (devices_[idx].is_free()) {
                devices_[idx].start_service(request_id, current_time_);
                requests_[request_id].set_service_start_time(current_time_);
                next_device_idx_ = (idx + 1) % devices_.size();
                
                double service_time = service_dist_(rng_);
                schedule_service_end(idx, request_id, current_time_ + service_time);
                device_found = true;
                break;
            }
        }
        
        if (!device_found) {
            if (buffer_.place_request(request_id)) {
                // Successfully placed in buffer
            } else {
                // Buffer full - D10O4: displace and place
                buffer_.displace_request();
                buffer_.place_request(request_id);
                metrics_.record_refusal(source_id);
            }
        }
        
        // Schedule next arrival for this source only
        if (metrics_.get_arrived() < config_.max_arrivals) {
            Event next_arrival(current_time_ + config_.sources[source_id].arrival_interval, 
                              EventType::arrival, 0, 0, source_id);
            calendar_.schedule(next_arrival);
        }
    }
}

void Simulator::handle_service_end(size_t device_id) {
    size_t finished_id = devices_[device_id].finish_service(current_time_);
    
    if (finished_id < requests_.size()) { // Valid request ID
        Request& req = requests_[finished_id];
        double time_in_system = current_time_ - req.get_arrival_time();
        double waiting_time = req.get_service_start_time() - req.get_arrival_time();
        double service_time = current_time_ - req.get_service_start_time();
        
        metrics_.record_completion(finished_id, time_in_system, waiting_time, service_time);
        metrics_.record_device_busy_time(device_id, service_time);
    }
    
    // D2B3: Check buffer for waiting requests
    if (!buffer_.is_empty()) {
        auto next_request = buffer_.take_request();
        if (next_request) {
            devices_[device_id].start_service(*next_request, current_time_);
            requests_[*next_request].set_service_start_time(current_time_);
            next_device_idx_ = (device_id + 1) % devices_.size();
            
            double service_time = service_dist_(rng_);
            schedule_service_end(device_id, *next_request, current_time_ + service_time);
        }
    }
}

void Simulator::schedule_next_arrival() {
    // Schedule next arrival for each source if global limit not reached
    if (metrics_.get_arrived() < config_.max_arrivals) {
        for (size_t i = 0; i < config_.sources.size(); ++i) {
            Event arrival_event(current_time_ + config_.sources[i].arrival_interval, 
                              EventType::arrival, 0, 0, i);
            calendar_.schedule(arrival_event);
        }
    }
}

void Simulator::schedule_service_end(size_t device_id, size_t request_id, double end_time) {
    Event service_end_event(end_time, EventType::service_end, request_id, device_id);
    calendar_.schedule(service_end_event);
}

Metrics Simulator::get_metrics() const {
    return metrics_;
}

double Simulator::get_current_time() const {
    return current_time_;
}

void Simulator::print_state() const {
    std::cout << "\n=== SIMULATION STATE ===" << std::endl;
    std::cout << "Time: " << current_time_ << std::endl;
    std::cout << "Calendar size: " << calendar_.get_size() << std::endl;
    std::cout << "Buffer: " << buffer_.get_size() << "/" << buffer_.get_capacity() << std::endl;
    
    std::cout << "Devices:" << std::endl;
    for (size_t i = 0; i < devices_.size(); ++i) {
        std::cout << "  Device " << i << ": " << (devices_[i].is_free() ? "FREE" : "BUSY");
        if (!devices_[i].is_free()) {
            std::cout << " (request " << devices_[i].get_current_request_id() << ")";
        }
        std::cout << std::endl;
    }
    
    std::cout << "Metrics:" << std::endl;
    std::cout << "  Arrived: " << metrics_.get_arrived() << std::endl;
    std::cout << "  Refused: " << metrics_.get_refused() << std::endl;
    std::cout << "  Completed: " << metrics_.get_completed() << std::endl;
    std::cout << "  P_ref: " << metrics_.get_refusal_probability() << std::endl;
    std::cout << "========================\n" << std::endl;
}

bool Simulator::is_finished() const {
    return (initialized_ && calendar_.is_empty()) || 
           metrics_.get_arrived() >= config_.max_arrivals ||
           current_time_ > config_.max_time;
}
