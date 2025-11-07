#include "sim/utils/ExponentialDistribution.h"

ExponentialDistribution::ExponentialDistribution(double intensity, uint32_t seed)
    : rng_(seed), dist_(intensity) {}

double ExponentialDistribution::generate() { return dist_(rng_); }

