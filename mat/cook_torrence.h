#ifndef CBLT_COOK_TORRENCE_H
#define CBLT_COOK_TORRENCE_H

#include "material.h"
#include <memory>

namespace cblt
{
    class CookTorrenceMaterial final : public Material {
    public:
        CookTorrenceMaterial(Color albedo, Color specular, Color emissive, float ior, float rough_, float metal);

        Color Sample(const Vec3 &incoming, Vec3 &outgoing, float &pdf, const HitInfo &collisionPt, std::shared_ptr<Sampler2D> &BRDF_sampler);
        Color BRDF(const Vec3 &incoming, const Vec3 &outgoing, const HitInfo &collision_pt);
        Color Emittance();
    private:

        void RandomUnitVectorInHemisphere(const Vec3 &bitangent, const Vec3 &norm, const Vec3 &tangent, Vec3 &result, std::shared_ptr<Sampler2D> generator);
        void RandomUnitVectorInGGX(const Vec3 &bitangent, const Vec3 &norm, const Vec3 &tangent, const Vec3 &incoming, Vec3 &result, float &pdf, std::shared_ptr<Sampler2D> generator);
        float GGXDisbritution(float cos_theta);
        float SmithGeometry(const Vec3 &omega, const Vec3 &normal, const Vec3 &halfway);
        float SchlickFresnel(float F0, float cos_theta);
        Color albedo_;
        Color specular_;
        Color emissive_;
        float ior_;
        float roughness_;
        float metalness_;

        float lambertian_pdf_;
    };
}
#endif  // CBLT_COOK_TORRENCE_H