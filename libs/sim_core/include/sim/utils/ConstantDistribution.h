#ifndef SIM_UTILS_CONSTANT_DISTRIBUTION_H_
#define SIM_UTILS_CONSTANT_DISTRIBUTION_H_

#include "sim/utils/IDistribution.h"

class ConstantDistribution : public IDistribution {
 public:
  explicit ConstantDistribution(double constant_value);
  ~ConstantDistribution() override = default;
  double generate() override;

 private:
  double constant_value_;
};

#endif  // SIM_UTILS_CONSTANT_DISTRIBUTION_H_

