#ifndef RANDOM_SAMPLER_H
#define RANDOM_SAMPLER_H

#include "sampler.h"

class RandomSampler2D : public Sampler2D {
public:
    RandomSampler2D();
    void NextSample(float &x, float &y);
};

#endif  // RANDOM_SAMPLER_H