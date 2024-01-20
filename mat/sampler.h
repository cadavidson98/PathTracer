#ifndef SAMPLER_H
#define SAMPLER_H

#include <cstdint>

namespace cblt
{
    class Sampler 
    {
    public:
        virtual void Next1D(float &x) = 0;
        virtual void Next2D(float &x, float &y) = 0;
        virtual void Seed(uint32_t seed) = 0;
    };
}
#endif  // SAMPLER_H