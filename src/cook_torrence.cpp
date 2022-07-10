#include "cook_torrence.h"
#include "matrix.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include "math_helpers.h"

CookTorrenceMaterial::CookTorrenceMaterial(Color albedo, Color specular, Color emissive, float ior, float rough, float metal) : 
albedo_(albedo), specular_(specular), emissive_(emissive), ior_(ior), roughness_(rough), metalness_(metal) {
    lambertian_pdf_ = 1.f / (2.f * M_PI); 
}

Color CookTorrenceMaterial::Sample(const Vec3 &incoming, Vec3 &outgoing, float &pdf, const HitInfo &collisionPt, std::shared_ptr<Sampler2D> BRDF_sampler) {
    // Illuminate using Cook-Torence reflectance model
    // First, determine which BRDF we need to use for this ray
    float x, y;
    BRDF_sampler->NextSample(x, y);
    float tot_luminance = 1.f;
    float percent_diffuse = 1.f - metalness_;
    float percent_spec = metalness_;
    if(true || x <= percent_diffuse) {
        // lambertian diffuse BTDF
        // start by finding a random outgoing direction
        Vec3 bitangent = collisionPt.norm.Cross(incoming).UnitVec();
        Vec3 tangent = collisionPt.norm.Cross(bitangent).UnitVec();
        RandomUnitVectorInHemisphere(bitangent, collisionPt.norm, tangent, outgoing, BRDF_sampler);
        pdf = lambertian_pdf_;
        //Color base = (albedo_map_) ? albedo_map_->sample(collisionPt.uv.x, collisionPt.uv.y) : albedo_;
        return BRDF(incoming, outgoing, collisionPt);
        // now find the albedo color at this point
        // return base * percent_diffuse * M_1_PI;
    }
    else {
        // specular
        /*float in_dot_n = incoming.Dot(collisionPt.norm);
        // get basis around reflection vector
        Vec3 reflect = (incoming - 2 * in_dot_n * collisionPt.norm).UnitVec();
        Vec3 w = reflect.Cross(collisionPt.norm).UnitVec();
        Vec3 u = w.Cross(reflect).UnitVec();
        // now we can take a random sample to get a glossy reflection
        float x, y;
        BRDF_sampler->NextSample(x, y);
        // map x, y to [-a/2, a/2]
        x = roughness_ * -.5f + roughness_ * x;
        y = roughness_ * -.5f + roughness_ * y;
        outgoing = (reflect + w * x + u * y).UnitVec();
        float rough_sqr = roughness_ * roughness_;
        // since we sample over a square and not a disk, this pdf is the area of a pyramid
        // this will be changed to a disk after evaluating whether this method of jitter rays
        // works
        pdf = rough_sqr + 2.f * roughness_ * std::sqrt((roughness_ * .5f) * (roughness_ * .5f) + 1.f);
        pdf = 1.f / pdf;
        return albedo_ * percent_spec;*/
    }
}

Color CookTorrenceMaterial::BRDF(const Vec3 &incoming, const Vec3 &outgoing, const HitInfo &collision_pt) {
    // lambertian diffuse BTDF
    Color diffuse = (albedo_map_) ? albedo_map_->sample(collision_pt.uv.x, collision_pt.uv.y) : albedo_;
    diffuse = diffuse / (RMth::PI_f);
    // specular
    float in_dot_n = incoming.Dot(collision_pt.norm);
    
    float rough_sqr = roughness_ * roughness_;
    float out_dot_h = outgoing.Dot(collision_pt.norm);
    Vec3 halfway = incoming + outgoing;
    halfway.Normalize();

    // Approximate microfacets using the GGX normal distribution function
    float n_dot_h = collision_pt.norm.Dot(halfway);
    float chi_h_n = static_cast<float>(n_dot_h > 0.f);
    float n_dot_h_sqr = n_dot_h * n_dot_h;
    float denom = n_dot_h_sqr * rough_sqr + (1.f - n_dot_h_sqr);
    float GGX = (rough_sqr * chi_h_n) / (static_cast<float>(M_PI) * denom * denom);
    // Geometric attenuation
    float n_dot_l = collision_pt.norm.Dot(outgoing);
    float n_dot_v = collision_pt.norm.Dot(incoming);
    float v_dot_h = incoming.Dot(halfway);
    float G = GeometryFunction(incoming, collision_pt.norm, halfway) * GeometryFunction(outgoing, collision_pt.norm, halfway);
    // Approximate geometry of microfacets using Schlick
    float F0 = (1.f - ior_) / (1.f + ior_);
    F0 *= F0;
    float F = F0 + (1.f - F0) * std::pow(v_dot_h, 5);
    Color reflective = specular_ * (GGX * G * F) / (4.f * n_dot_l * n_dot_v + .05f);
    return diffuse * (1.f - metalness_) + reflective * metalness_;
}
    
Color CookTorrenceMaterial::Emittance() {
    return emissive_;
}

void CookTorrenceMaterial::RandomUnitVectorInHemisphere(const Vec3 &bitangent, const Vec3 &normal, const Vec3 &tangent, 
                                                      Vec3 &result, std::shared_ptr<Sampler2D> generator) {
    // sart by getting a random unit vector in the unit (hemi)sphere
    float u1 = 0.f;
    float u2 = 0.f;
    // calculates 2 quasi-random numbers in the range 0-1 inclusive
    generator->NextSample(u1, u2);
    float radius = sqrtf(1.f - u1 * u1);
    float theta = 2.f * M_PI * u2;
    
    float x = radius * cos(theta);
    float z = radius * sin(theta);
    
    Vec3 rand_vec(x, u1, z);
    Matrix4x4 BNT(Vec4(bitangent, 0), Vec4(normal, 0), Vec4(tangent, 0), Vec4(0, 0, 0, 1));
    Vec4 out_vec = BNT * Vec4(rand_vec, 0);

    result.x = out_vec.x;
    result.y = out_vec.y;
    result.z = out_vec.z;
    result.Normalize();
    if(result.Dot(normal) < 0) {
        // make sure the sample is inside the hemisphere
        result = result * -1;
    }
}

float CookTorrenceMaterial::GeometryFunction(const Vec3 &omega, const Vec3 &normal, const Vec3 &halfway) {
    float w_dot_h = std::max(omega.Dot(halfway), 0.f);
    float w_dot_n = std::max(omega.Dot(normal), 0.f);
    
    float chi = static_cast<float>((w_dot_h / w_dot_n) > 0.f);
    float tan_sqr = (1.f - w_dot_h * w_dot_h) / (w_dot_h * w_dot_h);
    return (chi * 2.f) / (1.f + std::sqrt(1.f + roughness_ * roughness_ * tan_sqr));
}