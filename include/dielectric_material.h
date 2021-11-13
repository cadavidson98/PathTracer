#ifndef DIELECTRIC_MATERIAL_H
#define DIELECTRIC_MATERIAL_H

#include "material.h"
#include <memory>

class DielectricMaterial : public Material {
public:
    DielectricMaterial(Color albedo, Color specular, Color emissive, float ior, float rough_);
    Color ApplyPhongBlinn(const Vec3 &incoming, const Vec3 &outgoing, const Vec3 &normal);
    Color Sample(const Vec3 &incoming, Vec3 &outgoing, float &pdf, const Vec3 &toEye, const HitInfo &collisionPt, std::shared_ptr<Sampler2D> BRDF_sampler);
    Color Emittance();
private:

    void RandomUnitVectorInHemisphere(const Vec3 &bitangent, const Vec3 &norm, const Vec3 &tangent, Vec3 &result, std::shared_ptr<Sampler2D> generator);

    Color albedo_;
    Color specular_;
    Color emissive_;
    float ior_;
    float roughness_;
};

#endif  // DIELECTRIC_MATERIAL_H