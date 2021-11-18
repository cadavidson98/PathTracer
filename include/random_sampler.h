#ifndef RANDOM_SAMPLER_H
#define RANDOM_SAMPLER_H

#include "sampler.h"
#include <random>
class RandomSampler2D : public Sampler2D {
public:
    RandomSampler2D();
    void NextSample(float &x, float &y);
private:
    const float one_over_max_ = (1.0f / RAND_MAX);
};

#endif  // RANDOM_SAMPLER_H