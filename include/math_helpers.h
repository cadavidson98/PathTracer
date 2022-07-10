#ifndef MATH_HELPERS_H
#define MATH_HELPERS_H

// Rendering Math Namespace
namespace RMth {
    
    const float PI_f = 3.14159265f;
    const double PI_d = 3.14159265;
    
    constexpr float toRadians(float x) {
        return x * PI_f / 180.f;
    }

    constexpr double toRadians(double x) {
        return x * PI_d / 180.0;
    }

    constexpr float toDegrees(float x) {
        return x * 180.f / PI_f;
    }

    constexpr double toDegrees(double x) {
        return x * 180.0 / PI_d;
    }
}
#endif  // MATH_HELPERS_H