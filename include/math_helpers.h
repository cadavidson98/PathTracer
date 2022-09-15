#ifndef MATH_HELPERS_H
#define MATH_HELPERS_H

#include "vec.h"
// Rendering Math Namespace
namespace RMth {
    
    const float PI_f = 3.14159265f;
    const double PI_d = 3.14159265;
    
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

    template <class T>
    inline T sqr(T x) {
        return x * x;
    }

    template <class T>
    inline T lerp(T x, T y, float t) {
        return x + t * (y - x);
    }
}
#endif  // MATH_HELPERS_H