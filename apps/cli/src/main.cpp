#include <iostream>

#include "sim/core/Metrics.h"
#include "sim/core/Simulator.h"
#include "sim/core/SimulationConfig.h"

void print_state(const Simulator& simulator) {
  std::cout << "\n=== SIMULATION STATE ===" << std::endl;
  std::cout << "Time: " << simulator.get_current_time() << std::endl;
  std::cout << "Calendar size: " << simulator.get_calendar_size() << std::endl;
  
  const auto& buffer = simulator.get_buffer();
  std::cout << "Buffer: " << buffer.get_size() << "/" << buffer.get_capacity()
            << std::endl;

  std::cout << "Devices:" << std::endl;
  const auto& device_pool = simulator.get_device_pool();
  const auto& devices = device_pool.get_all_devices();
  for (size_t i = 0; i < devices.size(); ++i) {
    if (devices[i]) {
      std::cout << "  Device " << i << ": "
                << (devices[i]->is_free() ? "FREE" : "BUSY");
      if (!devices[i]->is_free()) {
        auto request = devices[i]->get_current_request();
        if (request) {
          std::cout << " (request " << request->get_id() << ")";
        }
      }
      std::cout << std::endl;
    }
  }

  const auto& metrics = simulator.get_metrics();
  std::cout << "Metrics:" << std::endl;
  std::cout << "  Arrived: " << metrics.get_arrived() << std::endl;
  std::cout << "  Refused: " << metrics.get_refused() << std::endl;
  std::cout << "  Completed: " << metrics.get_completed() << std::endl;
  std::cout << "  P_ref: " << metrics.get_refusal_probability() << std::endl;
  std::cout << "========================\n" << std::endl;
}

auto main() -> int {
  SimulationConfig config;
  config.num_devices = 3;
  config.buffer_capacity = 3;
  config.device_intensity = 0.3;  // μ - requests per time unit
  config.max_arrivals = 1000;
  config.seed = 52;

  config.sources.push_back({0, 3.0});
  config.sources.push_back({1, 4.0});
  config.sources.push_back({2, 5.0});

  Simulator simulator(config);

  std::cout << "=== STEP-BY-STEP DEBUG MODE ===" << std::endl;
  std::cout << "Commands: 's' = step, 'r' = run to end, 'q' = quit"
            << std::endl;

  char command;
  int step_count = 0;

  while (!simulator.is_finished()) {
    std::cout << "\nStep " << step_count << " - Enter command: ";
    command = getchar();

    if (command == 'q') {
      break;
    } else if (command == 'r') {
      std::cout << "Running to completion..." << std::endl;
      simulator.run();
      break;
    } else {  // TEMP
      simulator.step();
      print_state(simulator);
      step_count++;
    }
    // } else if (command == "s") {
    //     simulator.step();
    //     simulator.print_state();
    //     step_count++;
    // } else {
    //     std::cout << "Unknown command. Use 's', 'r', or 'q'" << std::endl;
    // }
  }

  std::cout << "\n=== FINAL RESULTS ===" << std::endl;
  Metrics metrics = simulator.get_metrics();
  std::cout << "arrived," << metrics.get_arrived() << std::endl;
  std::cout << "refused," << metrics.get_refused() << std::endl;
  std::cout << "completed," << metrics.get_completed() << std::endl;
  std::cout << "p_ref," << metrics.get_refusal_probability() << std::endl;
  std::cout << "avg_time_in_system," << metrics.get_avg_time_in_system()
            << std::endl;
  std::cout << "avg_waiting," << metrics.get_avg_waiting_time() << std::endl;
  std::cout << "avg_service," << metrics.get_avg_service_time() << std::endl;

  return 0;
}
