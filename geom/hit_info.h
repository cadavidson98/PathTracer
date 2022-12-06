#ifndef HIT_INFO_H
#define HIT_INFO_H
#include "math\vec.h"
#include "math\mat4.h"

#include <memory>

namespace cblt
{

    class Material;

    class HitInfo final
    {
        public:
        Vec3 norm;
        Vec3 pos;
        Vec2 uv;
        float hit_time;
        Mat4 shading_basis;
        std::shared_ptr<Material> m;
    };
}

#endif  // HIT_INFO_H