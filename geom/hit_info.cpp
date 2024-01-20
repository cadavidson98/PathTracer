#include "hit_info.h"
#include "math/constants.h"
#include "mat/image_lib.h"

#include "light/area_light.h"
#include "scene_prim.h"

namespace cblt
{

    HitInfo::HitInfo()
    {
        float hit_time = inf_F;
        shading_basis = Mat4(1.f);
        medium_ior  = 1.f;
        emissive = false;
        m = nullptr;
        geom = nullptr;
    }

    Color HitInfo::Emittance(const Vec3& to_light)
    {
        float pdf = 0.f;
        return (emission != nullptr) ? emission->Emission() : Color::GreyScale(0.f);
    }
}