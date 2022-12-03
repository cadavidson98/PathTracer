#ifndef HIT_INFO_H
#define HIT_INFO_H
#include "math\vec.h"
#include "math\mat4.h"

namespace cblt
{
    class HitInfo final
    {
        public:
        Vec3 norm;
        Vec3 pos;
        float hit_time;
        Mat4 shading_basis;
    };
}

#endif  // HIT_INFO_H