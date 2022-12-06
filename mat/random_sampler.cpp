#include "random_sampler.h"
#include <random>
#include <ctime>

namespace cblt
{
    RandomSampler2D::RandomSampler2D(unsigned int seed) {
        generator_.seed(seed);
        distribution = std::uniform_real_distribution<float>(0.0, 1.0);
    }

    void RandomSampler2D::NextSample(float &x, float &y) {
        x = distribution(generator_);
        y = distribution(generator_);
    }
}