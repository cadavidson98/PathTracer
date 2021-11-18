#include "random_sampler.h"
#include <random>
#include <ctime>
RandomSampler2D::RandomSampler2D() {
    srand(time(NULL));
}

void RandomSampler2D::NextSample(float &x, float &y) {
    x = rand() * one_over_max_;
    y = rand() * one_over_max_;
}