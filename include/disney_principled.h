#ifndef DISNEY_PRINCIPLED
#define DISNEY_PRINCIPLED

#include "material.h"

class DisneyPrincipledMaterial final : public Material {
public:
    DisneyPrincipledMaterial(Color & base_color, float subsurface, float metallic, float specular, 
        float specular_tint, float roughness, float anisotropic, float sheen, float sheen_tint,
        float clearcoat, float clearcoat_gloss);

    Color Sample(const Vec3 &incoming, Vec3 &outgoing, float &pdf, const HitInfo &collisionPt, std::shared_ptr<Sampler2D> BRDF_sampler) override;
    Color BRDF(const Vec3 &incoming, const Vec3 &outgoing, const HitInfo &collision_pt) override;
    Color Emittance() override;
private:
    Color base_;
    float subsrfc_;
    float metal_;
    float spec_;
    float spec_tint_;
    float rough_;
    float aniso_;
    float sheen_;
    float sheen_tint_;
    float clrcoat_;
    float clrcoat_gloss_;
};
#endif