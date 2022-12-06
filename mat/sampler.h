#ifndef SAMPLER_H
#define SAMPLER_H
namespace cblt
{
    class Sampler2D {
    public:
        virtual void NextSample(float &x, float &y) = 0;
    };
}
#endif  // SAMPLER_H