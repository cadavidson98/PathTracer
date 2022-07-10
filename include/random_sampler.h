#ifndef RANDOM_SAMPLER_H
#define RANDOM_SAMPLER_H

#include "sampler.h"
#include <random>
class RandomSampler2D : public Sampler2D {
public:
    RandomSampler2D(unsigned int seed = 1U);
    void NextSample(float &x, float &y);
private:
    std::mt19937 generator_;
    std::uniform_real_distribution<float> distribution;
};

#endif  // RANDOM_SAMPLER_H