#include "isotropic_material.h"
#include "matrix.h"
#define _USE_MATH_DEFINES
#include <math.h>

IsotropicMaterial::IsotropicMaterial(Color albedo, Color specular, Color emissive, float ior, float rough) : 
albedo_(albedo), specular_(specular), emissive_(emissive), ior_(ior), roughness_(rough) {
    lambertian_pdf_ = 1.f / (2.f * M_PI); 
}

Color IsotropicMaterial::ApplyPhongBlinn(const Vec3 &incoming, const Vec3 &outgoing, const Vec3 &normal) {
    return albedo_;
}

Color IsotropicMaterial::Sample(const Vec3 &incoming, Vec3 &outgoing, float &pdf, const Vec3 &to_eye, const HitInfo &collisionPt, std::shared_ptr<Sampler2D> BRDF_sampler) {
    // Illuminate using Cook-Torence reflectance model
    // First, determine which BRDF we need to use for this ray
    float x, y;
    BRDF_sampler->NextSample(x, y);
    float tot_luminance = albedo_.r + albedo_.g + albedo_.b + specular_.r + specular_.g + specular_.b;
    float percent_diffuse = (albedo_.r + albedo_.g + albedo_.b) / tot_luminance;
    float percent_spec = (specular_.r + specular_.g + specular_.b) / tot_luminance;
    if(x < percent_diffuse) {
        // lambertian diffuse BTDF
        // start by finding a random outgoing direction
        Vec3 bitangent = collisionPt.norm.Cross(incoming).UnitVec();
        Vec3 tangent = collisionPt.norm.Cross(bitangent).UnitVec();
        RandomUnitVectorInHemisphere(bitangent, collisionPt.norm, tangent, outgoing, BRDF_sampler);
        pdf = lambertian_pdf_;
        Color base = (albedo_map_) ? albedo_map_->sample(collisionPt.uv.x, collisionPt.uv.y) : albedo_;
        // now find the albedo color at this point
        return base * percent_diffuse * M_1_PI;
    }
    else {
        // specular
        float in_dot_n = incoming.Dot(collisionPt.norm);
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
        pdf = rough_sqr + 2.f * roughness_ * std::sqrt((roughness_ * .5) * (roughness_ * .5) + 1.f);
        pdf = 1.f / pdf;
        return specular_ * percent_spec;
        /*
        TODO - Finish the Cook-Torrence Illumination Model. This requires sampling lights in the scene
        float out_dot_h = outgoing.Dot(collisionPt.norm);
        Vec3 halfway = (to_light + to_eye);
        halfway.Normalize();
        // Approximate microfacets using the normal distribution function
        float n_dot_h_sqr = incoming.Dot(halfway);
        n_dot_h_sqr *= n_dot_h_sqr;
        float denom = n_dot_h_sqr * (rough_sqr - 1) + 1;
        float GCX = rough_sqr / (M_PI * denom * denom);
        // Approximate geometry of microfacets using Schlick
        float n_dot_v = to_eye.Dot(collisionPt.norm);
        float k = (roughness_ + 1) * (roughness_ + 1) / 8.f;
        */
    }
}

Color IsotropicMaterial::Emittance() {
    return emissive_;
}

void IsotropicMaterial::RandomUnitVectorInHemisphere(const Vec3 &bitangent, const Vec3 &normal, const Vec3 &tangent, 
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
