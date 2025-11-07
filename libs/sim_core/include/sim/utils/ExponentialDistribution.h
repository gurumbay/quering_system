#ifndef SIM_UTILS_EXPONENTIAL_DISTRIBUTION_H_
#define SIM_UTILS_EXPONENTIAL_DISTRIBUTION_H_

#include "sim/utils/IDistribution.h"
#include <cstdint>
#include <random>

class ExponentialDistribution : public IDistribution {
 public:
  ExponentialDistribution(double intensity, uint32_t seed);
  ~ExponentialDistribution() override = default;
  double generate() override;

 private:
  std::mt19937 rng_;
  std::exponential_distribution<double> dist_;
};

#endif  // SIM_UTILS_EXPONENTIAL_DISTRIBUTION_H_

