#ifndef CBLT_HIT_INFO_H
#define CBLT_HIT_INFO_H
#include "math\vec.h"
#include "math\mat4.h"

#include "mat/image_lib.h"

#include <memory>

namespace cblt
{

    class Material;
    class ScenePrim;
    class AreaLight;

    class HitInfo final
    {
        public:
        HitInfo();
        Color Emittance(const Vec3 &to_light);

        Vec3 norm;
        Vec3 pos;
        Vec2 uv;
        float hit_time;
        Mat4 shading_basis;
        float medium_ior;  // IOR for the medium which the ray was traveling through when it hit this surface
        bool emissive;
        std::shared_ptr<Material> m = nullptr;
        AreaLight *emission = nullptr;
        ScenePrim *geom = nullptr;
    };
}

#endif  // CBLT_HIT_INFO_H