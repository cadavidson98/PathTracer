#ifndef MATH_HELPERS_H
#define MATH_HELPERS_H

// Rendering Math Namespace
namespace RMth {
    
    constexpr const float  PI_f = 3.1415926535897932384626433832795028841971f;
    constexpr const double PI_d = 3.1415926535897932384626433832795028841971;
    
    constexpr const float INV_PI_f = 1.f / PI_f;
    constexpr const double INV_PI_d = 1.f / PI_d;

    template <class T>
    inline T toRadians(T x) {
        return x * PI_f / 180.f;
    }

    template <class T>
    inline T toDegrees(T x) {
        return x * 180.f / PI_f;
    }

    template <class T>
    inline T sqr(T x) {
        return x * x;
    }

    template <typename T>
    inline T lerp(T x, T y, float t) {
        return x + (y - x) * t;
    }
}
#endif  // MATH_HELPERS_H