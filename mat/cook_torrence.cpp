#include "cook_torrence.h"
#include "math/mat4.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "math/math_helpers.h"
#include "math/constants.h"
#include "shading_helpers.h"

namespace cblt
{
    CookTorrenceMaterial::CookTorrenceMaterial(Color albedo, Color specular, Color emissive, float ior, float rough, float metal) : 
    albedo_(albedo), specular_(specular), emissive_(emissive), ior_(ior), roughness_(rough*rough), metalness_(metal) {
        lambertian_pdf_ = INV_PI_f; 
    }

    Color CookTorrenceMaterial::Sample(const Vec3 &outgoing, Vec3 &incoming, float &pdf, const HitInfo &collisionPt, std::shared_ptr<Sampler> &BRDF_sampler) {
        // Illuminate using Cook-Torence reflectance model
        // First, determine which BRDF we need to use for this ray
        float x;
        BRDF_sampler->Next1D(x);
        float tot_luminance = 1.f;
        float percent_diffuse = 1.f - metalness_;
        float percent_spec = metalness_;

        Vec3 bitangent, tangent;
        OrthonormalBasis(collisionPt.norm, bitangent, tangent);

        float foobar;

        if(x <= percent_diffuse) {
            // lambertian diffuse BTDF with cosine weighted sampling
            // start by finding a random outgoing direction
            RandomUnitVectorInHemisphere(bitangent, collisionPt.norm, tangent, incoming, BRDF_sampler);
            pdf = Dot(collisionPt.norm, incoming) * lambertian_pdf_;
            return BRDF(incoming, outgoing, collisionPt, foobar) / percent_diffuse;
        }
        else {
            // specular
            RandomUnitVectorInGGX(bitangent, collisionPt.norm, tangent, outgoing, incoming, pdf, BRDF_sampler);
            if (Dot(collisionPt.norm, incoming) < 0) {
                pdf = 1.f;
                return Color::GreyScale(0.f);
            }
            else {
                return BRDF(incoming, outgoing, collisionPt, foobar) / percent_spec;
            }
        }
    }

    // nomenclature guide:
    // incoming -> omega_i -> toLight (L) -> The light arriving at this point
    // outgoing -> omega_o -> toEye (V) -> The light reflected at this point
    Color CookTorrenceMaterial::BRDF(const Vec3 &incoming, const Vec3 &outgoing, const HitInfo &collision_pt, float &pdf) {
        // lambertian diffuse BTDF
        Color diffuse = (albedo_map_) ? albedo_map_->sample(collision_pt.uv.x, collision_pt.uv.y) : albedo_;
        diffuse = diffuse / (cblt::PI_f);
        // specular
        float in_dot_n = Dot(incoming, collision_pt.norm);
        float out_dot_n = Dot(outgoing, collision_pt.norm);
        float rough_sqr = roughness_ * roughness_;

        Vec3 halfway = Normalize(incoming + outgoing);

        // Approximate microfacets using the GGX normal distribution function
        float n_dot_h = Dot(collision_pt.norm, halfway);
        float chi_h_n = static_cast<float>(n_dot_h > 0.f);
        float GGX = chi_h_n * GGXDisbritution(n_dot_h);
        // Geometric attenuation
        float in_dot_h = Dot(incoming, halfway);
        float G = SmithGeometry(incoming, collision_pt.norm, halfway) * SmithGeometry(outgoing, collision_pt.norm, halfway);
        // Approximate geometry of microfacets using Schlick
        float F0 = (1.f - ior_) / (1.f + ior_);
        F0 *= F0;
        float F = SchlickFresnel(F0, Dot(outgoing, halfway));
        //return Color(F, F, F);
        Color reflective = specular_ * (GGX * G) / (4.f * std::abs(in_dot_n) * std::abs(out_dot_n));
        return diffuse * (1.f - metalness_) + reflective * metalness_;
    }

    Color CookTorrenceMaterial::Emittance() {
        return emissive_;
    }

    void CookTorrenceMaterial::RandomUnitVectorInHemisphere(const Vec3 &bitangent, const Vec3 &normal, const Vec3 &tangent, 
                                                            Vec3 &result, std::shared_ptr<Sampler> generator) {
        // sart by getting a random unit vector in the unit disk
        float u1 = 0.f;
        float u2 = 0.f;
        // calculates 2 quasi-random numbers in the range 0-1 inclusive
        generator->Next2D(u1, u2);
        float radius = std::sqrtf(u1);
        float theta = 2.f * PI_f * u2;

        float x = radius * cos(theta);
        float z = radius * sin(theta);
        // use malley's method to project it onto the hemisphere
        Vec3 rand_vec(x, 1.f - x * x - z * z, z);
        Mat4 BNT(Vec4(bitangent, 0.f), Vec4(normal, 0.f), Vec4(tangent, 0.f), Vec4(0.f, 0.f, 0.f, 1));
        Vec4 out_vec = BNT * Vec4(rand_vec, 0);

        result = Normalize(Vec3(out_vec.x, out_vec.y, out_vec.z));
        
        if (Dot(result, normal) < 0) {
            // make sure the sample is inside the hemisphere
            result = result * -1.f;
        }
    }

    // Adapted from https://schuttejoe.github.io/post/ggximportancesamplingpart1/
    void CookTorrenceMaterial::RandomUnitVectorInGGX(const Vec3 &bitangent, const Vec3 &normal, const Vec3 &tangent, 
                                                     const Vec3 &outgoing, Vec3 &result, float &pdf, std::shared_ptr<Sampler> generator) {
        Mat4 Tan_To_World(Vec4(bitangent, 0.f), Vec4(normal, 0.f), Vec4(tangent, 0.f), Vec4(0.f, 0.f, 0.f, 1.f));
        Mat4 World_To_Tan = OrthoInverse(Tan_To_World);
        Vec4 out_tan = World_To_Tan * Vec4(outgoing, 0.f);
        Vec3 w_o = Vec3(out_tan.x, out_tan.y, out_tan.z);

        // Sample only in GGX distribution
        float u1 = 0.f;
        float u2 = 0.f;

        generator->Next2D(u1, u2);
        // Formula from: https://agraphicsguy.wordpress.com/2015/11/01/sampling-microfacet-brdf/
        float rough_sqr = roughness_ * roughness_;
        float theta = std::acos((1.f - u1) / (u1 * (rough_sqr - 1.f) + 1.f));
        float phi = 2.f * cblt::PI_f * u2;

        float cos_theta = std::cos(theta);
        float sin_theta = std::sin(theta);

        // convert from polar to cartesian
        Vec3 sample(sin_theta * std::cos(phi),
                    cos_theta,
                    sin_theta * std::sin(phi));
        sample = Normalize(sample);
        // we sampled a normal, but we want a reflection vector, so reflect
        float O_dot_S = Dot(w_o, sample);
        float O_dot_N = Dot(w_o, Y_axis_F);
        float S_dot_N = Dot(sample, Y_axis_F);
        if (O_dot_N * S_dot_N < 0.f) {
            sample = -sample;
            O_dot_S = -O_dot_S;
        }

        Vec3 w_i = Normalize(2.f * O_dot_S * sample - w_o);

        // calculate the pdf for the reflection vector in cartesian coords
        pdf = GGXDisbritution(cos_theta) * cos_theta / (4.f * O_dot_S);

        // take back from tangent space to world space
        Vec4 out_vec = Tan_To_World * Vec4(w_i, 0.f);
        result = Normalize(Vec3(out_vec.x, out_vec.y, out_vec.z));
    }

    float CookTorrenceMaterial::GGXDisbritution(float cos_theta) {
        float cos_sqr = cos_theta * cos_theta;
        float rough_sqr = roughness_ * roughness_;
        float denom = cos_sqr * (rough_sqr - 1.f) + 1.f;
        return rough_sqr / (cblt::PI_f * denom * denom);
    }

    float CookTorrenceMaterial::SmithGeometry(const Vec3 &omega, const Vec3 &normal, const Vec3 &halfway) {
        float w_dot_h = std::max(Dot(omega, halfway), 0.f);
        float w_dot_n = std::max(Dot(omega, normal), 0.f);

        float chi = static_cast<float>((w_dot_h / w_dot_n) > 0.f);
        float tan_sqr = (1.f - w_dot_h * w_dot_h) / (w_dot_h * w_dot_h);
        return (chi * 2.f) / (1.f + std::sqrt(1.f + roughness_ * roughness_ * tan_sqr));
    }

    float CookTorrenceMaterial::SchlickFresnel(float F0, float cos_theta) {
        return F0 + (1.f - F0) * std::pow(1.f - cos_theta, 5);
    }
}