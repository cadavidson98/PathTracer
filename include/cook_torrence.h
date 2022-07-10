#ifndef ISOTROPIC_MATERIAL_H
#define ISOTROPIC_MATERIAL_H

#include "material.h"
#include <memory>

class CookTorrenceMaterial : public Material {
public:
    CookTorrenceMaterial(Color albedo, Color specular, Color emissive, float ior, float rough_, float metal);
    
    Color Sample(const Vec3 &incoming, Vec3 &outgoing, float &pdf, const HitInfo &collisionPt, std::shared_ptr<Sampler2D> BRDF_sampler);
    Color BRDF(const Vec3 &incoming, const Vec3 &outgoing, const HitInfo &collision_pt);
    Color Emittance();
private:

    void RandomUnitVectorInHemisphere(const Vec3 &bitangent, const Vec3 &norm, const Vec3 &tangent, Vec3 &result, std::shared_ptr<Sampler2D> generator);
    float GeometryFunction(const Vec3 &omega, const Vec3 &normal, const Vec3 &halfway);
    Color albedo_;
    Color specular_;
    Color emissive_;
    float ior_;
    float roughness_;
    float metalness_;

    float lambertian_pdf_;
};

#endif  // ISOTROPIC_MATERIAL_H