#ifndef MATH_CONSTANTS_H
#define MATH_CONSTANTS_H
#include "vec.h"
#include "mat4.h"

#include <cmath>
#include <limits>

namespace cblt
{
    // scalar constants
    const float inf_F = std::numeric_limits<float>::infinity();
    constexpr float minus_inf_F = -std::numeric_limits<float>::infinity();
    const float eps_zero_F = 1.e-6f;
    // vector constants
    const Vec3 X_axis_F = {1.f, 0.f, 0.f};
    const Vec3 Y_axis_F = {0.f, 1.f, 0.f};
    const Vec3 Z_axis_F = {0.f, 0.f, 1.f};

    // matrix constants
    const Mat4 Identity_F = Mat4(1.f);
}

#endif  // MATH_CONSTANTS_H