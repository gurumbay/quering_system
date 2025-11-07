#include "sim/utils/ConstantDistribution.h"

ConstantDistribution::ConstantDistribution(double constant_value)
    : constant_value_(constant_value) {}

double ConstantDistribution::generate() { return constant_value_; }

