#ifndef SAMPLE_HELPERS
#define SAMPLE_HEADERS

#include "math_helpers.h"
#include "shading_helpers.h"
#include "matrix.h"
#include "sampler.h"

#include <memory>

namespace RMth
{
    inline Vec3 RandomUnitVectorInHemisphere(const Vec3 &bitangent, const Vec3 &normal, const Vec3 &tangent, 
                                             std::shared_ptr<Sampler2D> generator)
    {
        // start by getting a random unit vector in the unit disk
        float u1 = 0.f;
        float u2 = 0.f;
        // calculates 2 quasi-random numbers in the range 0-1 inclusive
        generator->NextSample(u1, u2);
        float radius = std::sqrtf(std::max(0.f, 1.f - sqr(u1)));
        float theta = 2.f * RMth::PI_f * u2;

        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);
        // use malley's method to project it onto the hemisphere
        Vec3 rand_vec(x, u1, z);
        Matrix4x4 BNT(Vec4(bitangent, 0.f), Vec4(normal, 0.f), Vec4(tangent, 0.f), Vec4(0.f, 0.f, 0.f, 1.f));
        Vec4 out_vec = BNT * Vec4(rand_vec, 0);

        Vec3 result = {out_vec.x, out_vec.y, out_vec.z};
        result.Normalize();
        if(result.Dot(normal) < 0) {
            // make sure the sample is inside the hemisphere
            result = result * -1.f;
        }
        return result;
    }

    // Adapted from https://schuttejoe.github.io/post/ggximportancesamplingpart1/
    inline Vec3 RandomUnitVectorInGGX(const Vec3 &bitangent, const Vec3 &normal, const Vec3 &tangent, const Vec3 &outgoing, 
                                      const float alpha, float &pdf, const float u1, const float u2) {
        Matrix4x4 Tan_To_World(Vec4(bitangent, 0.f), Vec4(normal, 0.f), Vec4(tangent, 0.f), Vec4(0.f, 0.f, 0.f, 1.f));
        Matrix4x4 World_To_Tan = Matrix4x4::Invert(Tan_To_World);
        Vec4 out_tan = World_To_Tan * Vec4(outgoing, 0.f);
        Vec3 w_o = { out_tan.x, out_tan.y, out_tan.z };

        // Formula from: https://agraphicsguy.wordpress.com/2015/11/01/sampling-microfacet-brdf/
        float alpha_sqr = alpha * alpha;
        float theta = std::acos((1.f - u1) / (u1 * (alpha_sqr - 1.f) + 1.f));
        float phi = 2.f * RMth::PI_f * u2;

        float cos_theta = std::cos(theta);
        float sin_theta = std::sin(theta);

        // convert from polar to cartesian
        Vec3 sample = { sin_theta * std::cos(phi),
                        cos_theta,
                        sin_theta * std::sin(phi) };
        sample.Normalize();
        // we sampled a normal, but we want a reflection vector, so reflect
        float O_dot_S = w_o.Dot(sample);
        float O_dot_N = w_o.Dot(Vec3(0.f, 1.f, 0.f));
        float S_dot_N = sample.Dot(Vec3(0.f, 1.f, 0.f));
        if (O_dot_N * S_dot_N < 0.f) {
            sample = -sample;
            O_dot_S = -O_dot_S;
        }

        Vec3 w_i = 2.f * O_dot_S * sample - w_o;
        w_i.Normalize();
        // calculate the pdf for the reflection vector in cartesian coords
        pdf = GGX(cos_theta, alpha) * cos_theta / (4.f * O_dot_S);
    
        // take back from tangent space to world space
        Vec4 out_vec = Tan_To_World * Vec4(w_i, 0.f);
        Vec3 result = { out_vec.x, out_vec.y, out_vec.z };
        return result.UnitVec();
}

    inline Vec3 RandomUnitVectorInGGXAniso(const Vec3 &bitangent, const Vec3 &normal, const Vec3 &tangent, const Vec3 &outgoing, 
                                      const float alpha_x, const float alpha_y, float &pdf, const float u1, const float u2) {
        Matrix4x4 Tan_To_World(Vec4(bitangent, 0.f), Vec4(normal, 0.f), Vec4(tangent, 0.f), Vec4(0.f, 0.f, 0.f, 1.f));
        Matrix4x4 World_To_Tan = Matrix4x4::Invert(Tan_To_World);
        Vec4 out_tan = World_To_Tan * Vec4(outgoing, 0.f);
        Vec3 w_o = { out_tan.x, out_tan.y, out_tan.z };

        // Formula from: https://agraphicsguy.wordpress.com/2018/07/18/sampling-anisotropic-microfacet-brdf/
        float phi = std::atan((alpha_y / alpha_x) * std::tan(2 * PI_f * u1 + .5f * PI_f));
        phi += PI_f * (u1 > .5f);
        
        float alpha_sqr = sqr(std::cos(phi) / alpha_x) + sqr(std::sin(phi) / alpha_y);
        float tan_sqr = u2 / ((1.f - u2) * alpha_sqr);

        float theta = std::atan(std::sqrt(tan_sqr));

        float cos_theta = std::cos(theta);
        float sin_theta = std::sin(theta);
        float cos_phi = std::cos(phi);
        float sin_phi = std::sin(phi);
        // convert from polar to cartesian
        Vec3 sample = { sin_theta * cos_phi,
                        cos_theta,
                        sin_theta * sin_phi };
        sample.Normalize();
        // we sampled a normal, but we want a reflection vector, so reflect
        float O_dot_S = w_o.Dot(sample);
        float O_dot_N = w_o.y;
        float S_dot_N = sample.y;
        if (O_dot_N * S_dot_N < 0.f) {
            sample = -sample;
            O_dot_S = -O_dot_S;
        }

        Vec3 w_i = 2.f * O_dot_S * sample - w_o;
        w_i.Normalize();
        // calculate the pdf for the reflection vector in cartesian coords
        // float pdf_other = GGX(sample.y, alpha_x) * cos_theta / (4.f * O_dot_S);
        pdf = GGX_aniso(alpha_x, alpha_y, sample.x, sample.z, sample.y) * cos_theta / (4.f * O_dot_S);
    
        // take back from tangent space to world space
        Vec4 out_vec = Tan_To_World * Vec4(w_i, 0.f);
        Vec3 result = { out_vec.x, out_vec.y, out_vec.z };
        return result.UnitVec();
    }

    // A Simpler and Exact Sampling Routine for the GGX Distribution of Visible Normals by Eric Heitz
    // https://hal.archives-ouvertes.fr/hal-01509746/document
    inline Vec3 SampleVisibleGGX(const Vec3 &outgoing, const float a_x, const float a_y, const float u1, const float u2)
    {
        // Stretch Halfway Vector
        Vec3 v = { a_x * outgoing.x, outgoing.y, a_y * outgoing.z };
        v.Normalize();

        // Construct Orthonormal Basis: TODO- try replacing with RMth::OrthonormalBasis()
        Vec3 bitangent = (v.y < 0.999f) ? (v.Cross(Vec3(0.f, 1.f, 0.f))).UnitVec() : Vec3(1.f, 0.f, 0.f);
        Vec3 tangent = bitangent.Cross(v);

        // sample uniform hemisphere using polar coords
        float r = std::sqrt(u1);
        
        float a = 1.f / (1.f + v.y);
        float phi = (u2 < a) ? (u2 / a) * PI_f : PI_f + (u2 - a) / (1.f - a) * PI_f;
        float t1 = r * std::cos(phi);
        float t2 = r * std::sin(phi) * ((u2 < a) ? 1.f : v.y);
        
        Vec3 normal = bitangent * t1 + tangent * t2 + v * std::sqrt(std::max(0.f, 1.f - sqr(t1) - sqr(t2)));
        // unstretch
        Vec3 normal_e = { a_x * normal.x, std::max(0.f, normal.y), a_y * normal.z };
        normal_e.Normalize();
        return normal_e;
    }
}

#endif