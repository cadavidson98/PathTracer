#ifndef CBLT_GLOSSY_H
#define CBLT_GLOSSY_H

#include "material.h"

namespace cblt
{
    class GlossyMaterial : public Material
    {
        public:
        GlossyMaterial(Color base, float roughness, float ior = 1.45f);
        virtual Color Sample(const Vec3 &outgoing, Vec3 &incoming, float &pdf, const HitInfo &collisionPt, std::shared_ptr<Sampler> &BRDF_sampler) override;
        virtual Color BRDF(const Vec3 &outgoing, const Vec3 &incoming, const HitInfo &collision_pt, float &pdf) override;
        virtual Color Emittance() override;
        private:
        Color base_;
        float rough_sqr_;
        float ior_;
    };
}

#endif  // CBLT_GLOSSY_H