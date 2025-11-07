#ifndef SIM_UTILS_I_DISTRIBUTION_H_
#define SIM_UTILS_I_DISTRIBUTION_H_

class IDistribution {
 public:
  virtual ~IDistribution() = default;
  virtual double generate() = 0;
};

#endif  // SIM_UTILS_I_DISTRIBUTION_H_

