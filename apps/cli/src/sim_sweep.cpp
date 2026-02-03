#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

#include "sim/simulator/SimulationConfig.h"
#include "sim/simulator/Simulator.h"

using namespace std;

int main(int argc, char** argv) {
  // Configurable runtime parameters
  size_t max_arrivals = 10000; // default per-report
  if (argc > 1) {
    try {
      max_arrivals = static_cast<size_t>(stoul(argv[1]));
    } catch (...) {
      cerr << "Invalid max_arrivals argument, using default 10000" << endl;
    }
  }

  // Grid ranges (from report)
  vector<size_t> sensors_range;
  for (size_t n = 4; n <= 20; ++n) sensors_range.push_back(n);

  vector<size_t> interval_ms_vals;
  for (size_t t = 50; t <= 500; t += 50) interval_ms_vals.push_back(t);

  vector<size_t> devices_range;
  for (size_t m = 1; m <= 6; ++m) devices_range.push_back(m);

  vector<int> device_types = {1, 2, 3};
  vector<size_t> buffer_sizes = {8, 16, 24, 32, 40};

  // Device type properties
  auto type_mean_ms = [](int type) {
    switch (type) {
      case 1: return 120.0;
      case 2: return 90.0;
      case 3: return 60.0;
      default: return 120.0;
    }
  };
  auto type_price = [](int type) {
    switch (type) {
      case 1: return 6000ul;
      case 2: return 9000ul;
      case 3: return 15000ul;
      default: return 6000ul;
    }
  };

  ofstream out("sweep_results.csv");
  out << "sensors;interval_ms;devices;device_type;buffer_size;max_arrivals;p_ref;avg_time_ms;utilization;cost;passes" << '\n';

  size_t total_configs = sensors_range.size() * interval_ms_vals.size() * devices_range.size() * device_types.size() * buffer_sizes.size();
  size_t cfg_count = 0;

  for (auto sensors : sensors_range) {
    for (auto interval_ms : interval_ms_vals) {
      for (auto devices : devices_range) {
        for (auto dtype : device_types) {
          for (auto buf : buffer_sizes) {
            ++cfg_count;
            if (cfg_count % 50 == 0 || cfg_count == 1) {
              cout << "Running config " << cfg_count << " / " << total_configs << "..." << endl;
            }

            SimulationConfig config;
            config.buffer_capacity = buf;
            config.max_arrivals = max_arrivals;
            config.seed = static_cast<uint32_t>(12345 + cfg_count);

            // sources: equal intervals
            for (size_t i = 0; i < sensors; ++i) {
              config.sources.push_back({i, static_cast<double>(interval_ms), DistributionType::Constant});
            }

            // devices: all of same type (per-report assumption)
            double mean_ms = type_mean_ms(dtype);
            double mu = 1.0 / mean_ms; // rate parameter for exponential
            for (size_t j = 0; j < devices; ++j) {
              config.devices.push_back({j, mu, DistributionType::Exponential});
            }

            // Run simulation
            Simulator sim(config);
            sim.run();

            auto metrics = sim.get_metrics();
            double p_ref = metrics.get_refusal_probability();
            double avg_time = metrics.get_avg_time_in_system();

            // compute average utilization across devices
            double utilization_sum = 0.0;
            for (size_t d = 0; d < devices; ++d) {
              utilization_sum += metrics.get_device_utilization(d, sim.get_current_time());
            }
            double avg_util = utilization_sum / static_cast<double>(devices);

            // cost
            unsigned long device_price = type_price(dtype);
            unsigned long cost = devices * device_price + (buf / 8) * 800ul;

            bool passes = (p_ref <= 0.10) && (avg_time <= 200.0) && (avg_util >= 0.90);

            out << sensors << ';' << interval_ms << ';' << devices << ';' << dtype << ';' << buf << ';' << max_arrivals << ';'
                << fixed << setprecision(6) << p_ref << ';' << avg_time << ';' << avg_util << ';' << cost << ';' << (passes ? "yes" : "no") << '\n';
          }
        }
      }
    }
  }

  out.close();
  cout << "Sweep completed. Results written to sweep_results.csv" << endl;

  return 0;
}
