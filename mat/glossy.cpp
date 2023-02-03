#include "glossy.h"

#include "shading_helpers.h"
#include "sampling_helpers.h"

namespace cblt
{
    GlossyMaterial::GlossyMaterial(Color base, float roughness, float ior)
    : base_(base), rough_sqr_(roughness * roughness), ior_(ior)
    {

    }

    Color GlossyMaterial::Sample(const Vec3 &outgoing, Vec3 &incoming, float &pdf, const HitInfo &collision_pt, std::shared_ptr<Sampler> &BRDF_sampler)
    {
        float u1, u2;
        Vec3 tangent, bitangent;
        
        BRDF_sampler->Next2D(u1, u2);
        OrthonormalBasis(collision_pt.norm, bitangent, tangent);
        Mat4 Tan_To_World(Vec4(bitangent, 0.f), Vec4(collision_pt.norm, 0.f), Vec4(tangent, 0.f), Vec4(0.f, 0.f, 0.f, 1.f));
        Mat4 World_To_Tan = OrthoInverse(Tan_To_World);
        Vec4 out_tan = World_To_Tan * Vec4(outgoing, 0.f);
        Vec3 w_o(out_tan.x, out_tan.y, out_tan.z);

        Vec3 sample_h = SampleVisibleGGX(w_o, rough_sqr_, rough_sqr_, u1, u2);
        // reflect about sampled halfway to get the incoming light vector
        float o_dot_s = Dot(w_o, sample_h);
        float o_dot_n = w_o.y;
        float s_dot_n = sample_h.y;

        Vec3 w_i = Normalize(2.f * o_dot_s * sample_h - w_o);
        float D_aniso = GGX(s_dot_n, rough_sqr_);
        float G1_aniso = SmithPartialGeom(outgoing, sample_h, rough_sqr_);
        pdf = (D_aniso * G1_aniso) / (4.f * o_dot_n);

        // take back from tangent space to world space
        Vec4 out_vec = Tan_To_World * Vec4(w_i, 0.f);
        incoming = Normalize(Vec3(out_vec.x, out_vec.y, out_vec.z));
        
        return BRDF(incoming, outgoing, collision_pt, pdf);
    }

    Color GlossyMaterial::BRDF(const Vec3 &incoming, const Vec3 &outgoing, const HitInfo &collision_pt, float &pdf)
    {
        
        float o_dot_n = Dot(incoming, collision_pt.norm);
        float i_dot_n = Dot(outgoing, collision_pt.norm);
        
        if (i_dot_n < 0.f || o_dot_n < 0.f)
        {
            // no response below the hemisphere - transmission is for glass
            pdf = 0.f;
            return Color::GreyScale(0.f);
        }

        Vec3 halfway = Normalize(incoming + outgoing);
        float n_dot_h = Dot(collision_pt.norm, halfway);
        float o_dot_h = Dot(outgoing, halfway);
        float i_dot_h = Dot(incoming, halfway);

        float chi_h_n = static_cast<float>(n_dot_h > 0.f);

        // Approximate microfacets using the GGX normal distribution function
        float D = GGX(n_dot_h, rough_sqr_);
        
        // Geometric attenuation - Smith Partial Geometry
        float G1 = SmithPartialGeom(outgoing, collision_pt.norm, rough_sqr_);
        float G = G1 * SmithPartialGeom(incoming, collision_pt.norm, rough_sqr_);
        
        pdf = (D * G1) / (4.f * o_dot_n);
        // Approximate geometry of microfacets using Schlick
        float F0 = (1.f - ior_) / (1.f + ior_);
        F0 *= F0;
        float F = FresnelSchlick(F0, o_dot_h);
        return base_ * (chi_h_n * D * F * G) / (4.f * std::abs(i_dot_n) * std::abs(o_dot_n));
    }

    Color GlossyMaterial::Emittance()
    {
        return Color::GreyScale(0.f);
    }
}