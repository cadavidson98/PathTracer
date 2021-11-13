#include "dielectric_material.h"
#include "matrix.h"
#define _USE_MATH_DEFINES
#include <math.h>
#define M_1_2PI 1.f / (2.f * M_PI)
DielectricMaterial::DielectricMaterial(Color albedo, Color specular, Color emissive, float ior, float rough) : 
albedo_(albedo), specular_(specular), emissive_(emissive), ior_(ior), roughness_(rough) {

}

Color DielectricMaterial::ApplyPhongBlinn(const Vec3 &incoming, const Vec3 &outgoing, const Vec3 &normal) {
    return albedo_;
}

Color DielectricMaterial::Sample(const Vec3 &incoming, Vec3 &outgoing, float &pdf, const Vec3 &to_eye, const HitInfo &collisionPt, std::shared_ptr<Sampler2D> BRDF_sampler) {
    // Illuminate using Cook-Torence reflectance model
    // First, determine which BRDF we need to use for this ray
    float x, y;
    BRDF_sampler->NextSample(x, y);
    // use the x to check: [0,0.5) is BTDF and [0.5,1) is BSDF
    if(true) {
        // lambertian diffuse BTDF
        // start by finding a random outgoing direction
        Vec3 bitangent = collisionPt.norm.Cross(incoming).UnitVec();
        Vec3 tangent = collisionPt.norm.Cross(bitangent).UnitVec();
        RandomUnitVectorInHemisphere(bitangent, collisionPt.norm, tangent, outgoing, BRDF_sampler);
        pdf = M_1_2PI;
        // now find the albedo color at this point
        return albedo_ * M_1_PI;
    }
    else {
        // specular
        float in_dot_n = incoming.Dot(collisionPt.norm);
        float out_dot_h = outgoing.Dot(collisionPt.norm);
        Vec3 halfway = (incoming + to_eye);
        halfway.Normalize();
        // Approximate microfacets using the normal distribution function
        float rough_sqr = roughness_ * roughness_;
        float n_dot_h_sqr = incoming.Dot(halfway);
        n_dot_h_sqr *= n_dot_h_sqr;
        float denom = n_dot_h_sqr * (rough_sqr - 1) + 1;
        float GCX = rough_sqr / (M_PI * denom * denom);
        // Approximate geometry of microfacets using Schlick
        float n_dot_v = to_eye.Dot(collisionPt.norm);
        float k = (roughness_ + 1) * (roughness_ + 1) / 8.f;
        // Check if there is light traveling to this point
        
        // TODO still going to assume perfect mirror surface for reflection, but I think I would use
        // roughness_ as a "error factor" to jitter the reflections and make it glossy
        // Get a basis around
        outgoing = incoming - 2 * in_dot_n * collisionPt.norm;
    }
    return albedo_;
}

Color DielectricMaterial::Emittance() {
    return emissive_;
}

void DielectricMaterial::RandomUnitVectorInHemisphere(const Vec3 &bitangent, const Vec3 &normal, const Vec3 &tangent, 
                                                      Vec3 &result, std::shared_ptr<Sampler2D> generator) {
    // sart by getting a random unit vector in the unit (hemi)sphere
    float u1 = 0;
    float u2 = 0;
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
        result = result * -1;
    }
    return;
    /*
    // sample concentric uniform disk, and extrude to get a hemisphere
    u1 = 2.f * u1 - 1.f;
    u2 = 2.f * u2 - 1.f;
    // handle the division by 0
    float radius;
    float theta;
    if(u1 == 0.f && u2 == 0.f) {
        radius = 0.f;
        theta = 0.f;
    }
    else {
        // concentric map the square to the disk
        if(std::abs(u1) > std::abs(u2)) {
            radius = u1;
            theta = M_PI_4 * (u2 / u1);
        }
        else {
            radius = u2;
            theta = M_PI_2 - (M_PI_4 * (u1 / u2));
        }
    }
    float x = radius * std::cos(theta);
    float y = radius * std::sin(theta);
    float z = std::sqrt(std::max(0.f, 1.f - x * x - y * y));
    Vec3 rand_vec(x, y, z);
    Matrix4x4 BNT(Vec4(bitangent, 0), Vec4(normal, 0), Vec4(tangent, 0), Vec4(0, 0, 0, 1));
    Vec4 out_vec = BNT * Vec4(rand_vec, 0);
    out_vec.Normalize();
    result.x = out_vec.x;
    result.y = out_vec.y;
    result.z = out_vec.z;*/
}
