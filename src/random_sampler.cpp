#include "random_sampler.h"
#include <random>
#include <ctime>
RandomSampler2D::RandomSampler2D() {
    srand(time(NULL));
}

void RandomSampler2D::NextSample(float &x, float &y) {
    x = rand() / static_cast<float>(RAND_MAX);
    y = rand() / static_cast<float>(RAND_MAX);
}