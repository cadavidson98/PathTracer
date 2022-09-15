#include "disney_principled.h"
#include "shading_helpers.h"
#include "math_helpers.h"

DisneyPrincipledMaterial::DisneyPrincipledMaterial(Color & base_color, float subsurface, float metallic, float specular, 
        float specular_tint, float roughness, float anisotropic, float sheen, float sheen_tint,
        float clearcoat, float clearcoat_gloss) :
        base_(base_color), subsrfc_(subsurface), metal_(metallic), spec_(specular),
        spec_tint_(specular_tint), rough_(roughness), aniso_(anisotropic), sheen_(sheen),
        sheen_tint_(sheen_tint), clrcoat_(clearcoat), clrcoat_gloss_(clearcoat_gloss)
{
}

Color DisneyPrincipledMaterial::Sample(const Vec3 &incoming, Vec3 &outgoing, float &pdf, const HitInfo &collision_pt, std::shared_ptr<Sampler2D> BRDF_sampler)
{

}

Color DisneyPrincipledMaterial::BRDF(const Vec3 &incoming, const Vec3 &outgoing, const HitInfo &collision_pt)
{
    Vec3 X, Y;
    RMth::OrthonormalBasis(collision_pt.norm, X, Y);

    Vec3 halfway = (incoming + outgoing).UnitVec();
    float n_dot_h = halfway.Dot(collision_pt.norm);
    float h_dot_x = halfway.Dot(X);
    float h_dot_y = halfway.Dot(Y);

    float n_dot_o = outgoing.Dot(collision_pt.norm);
    float n_dot_i = incoming.Dot(collision_pt.norm);
    float h_dot_i = incoming.Dot(halfway);
    

    float fresnel_i = std::pow(1.f - n_dot_i, 5);
    float fresnel_o = std::pow(1.f - n_dot_o, 5);
    
    // Disney Diffuse
    float fresnel_d90 = 0.5f + 2.f * rough_ * RMth::sqr(h_dot_i);
    float diffuse = RMth::lerp(1.f, fresnel_d90, fresnel_i) * RMth::lerp(1.f, fresnel_d90, fresnel_o);

    float fresnel_ss90 = rough_ * RMth::sqr(h_dot_i); 
    float subsurface = 1.25f * 
        (RMth::lerp(1.f, fresnel_ss90, n_dot_i) * RMth::lerp(1.f, fresnel_ss90, n_dot_o) *
        (1.f / (std::abs(n_dot_i)) + std::abs(n_dot_o) - 0.5f) + .5f);
    
    Color Disney_diffuse = base_ * (1.f / RMth::PI_f) * (diffuse * (1.f - subsrfc_) + subsurface * subsrfc_);

    // Disney Specular
    float aspect = std::sqrt(1.f - 0.9f * aniso_);
    float alpha_x = std::max(.0001f, RMth::sqr(rough_) / aspect);
    float alpha_y = std::max(.0001f, RMth::sqr(rough_) * aspect);

    float D = RMth::GGX_aniso(alpha_x, alpha_y, h_dot_x, h_dot_y, n_dot_h);
    float G = RMth::SmithPartialGeom() * RMth::SmithPartialGeom();
}

Color Emittance()
{
    return Color(0.f, 0.f, 0.f);
}