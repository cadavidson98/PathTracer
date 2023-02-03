#include "lambertian.h"

#include "shading_helpers.h"
#include "sampling_helpers.h"

namespace cblt
{
    LambertianMaterial::LambertianMaterial(Color base) : base_(base)
    {

    }

    Color LambertianMaterial::Sample(const Vec3 &outgoing, Vec3 &incoming, float &pdf, const HitInfo &collisionPt, std::shared_ptr<Sampler> &BRDF_sampler)
    {
        float u1, u2;
        Vec3 tangent, bitangent;
        
        BRDF_sampler->Next2D(u1, u2);
        OrthonormalBasis(collisionPt.norm, bitangent, tangent);
        
        incoming = RandomUnitVectorInHemisphere(bitangent, collisionPt.norm, tangent, pdf, u1, u2);
        return BRDF(incoming, outgoing, collisionPt, pdf);
    }

    Color LambertianMaterial::BRDF(const Vec3 &incoming, const Vec3 &outgoing, const HitInfo &collision_pt, float &pdf)
    {
        if (Dot(incoming, collision_pt.norm) <= 0.f)
        {
            pdf = 0.f;
            return Color::GreyScale(0.f);
        }
        else
        {
            pdf = .5f * INV_PI_f;
            return base_ * INV_PI_f;
        }
    }

    Color LambertianMaterial::Emittance()
    {
        return Color::GreyScale(0.f);
    }
}