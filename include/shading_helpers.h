#ifndef SHADE_HELPERS
#define SHADE_HELPERS

#include "math_helpers.h"
#include <algorithm>
#include <cmath>

namespace RMth {
    inline float GGX(float cos_theta, float alpha)
    {
        float cos_sqr = cos_theta * cos_theta;
        float rough_sqr = alpha * alpha;
        float denom = cos_sqr * (rough_sqr - 1.f) + 1.f;
        return rough_sqr / (RMth::PI_f * denom * denom);
    }

    inline float GGX_aniso(float alpha_x, float alpha_y, float h_dot_x, float h_dot_y, float h_dot_z)
    {
        float denom = PI_f * alpha_x * alpha_y * sqr(sqr(h_dot_x / alpha_x) + sqr(h_dot_y / alpha_y) + sqr(h_dot_z));
        return 1.f / denom;
    }

    inline float SmithPartialGeom(const Vec3 &omega, const Vec3 &normal, const Vec3 &halfway, float alpha)
    {
        float w_dot_h = std::max(omega.Dot(halfway), 0.f);
        float w_dot_n = std::max(omega.Dot(normal), 0.f);
    
        float chi = static_cast<float>((w_dot_h / w_dot_n) > 0.f);
        float tan_sqr = (1.f - w_dot_h * w_dot_h) / (w_dot_h * w_dot_h);
        return (chi * 2.f) / (1.f + std::sqrt(1.f + alpha * alpha * tan_sqr));
    }

    inline float SmithPartialGeomAniso(float alpha_x, float alpha_y, const Vec3&w, const Vec3 &X, const Vec3 &Y, const Vec3 &Z)
    {
        float denom = 1.f + std::sqrt(1.f + );
        return 2.f / denom;
    }

    inline float FresnelSchlick(float F0, float cos_theta) 
    {
        return F0 + (1.f - F0) * std::pow(1.f - cos_theta, 5);
    }

    inline void OrthonormalBasis(const Vec3 &normal, Vec3 &tangent, Vec3 &bitangent)
    {
        float sign = std::copysign(1.f, normal.z);
        float a = -1.f / (sign + normal.z);
        float b = normal.x * normal.y * a;
        tangent = Vec3(1.f + sign * normal.x * normal.x * a, sign * b, -sign * normal.x);
        bitangent = Vec3(b, sign + normal.y * normal.y * a, -normal.y);
    }
}

#endif