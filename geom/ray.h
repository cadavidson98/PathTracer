#ifndef CBLT_RAY_H
#define CBLT_RAY_H

#include "math/vec.h"

namespace cblt
{
    struct Ray
    {
        Vec3 pos;
        Vec3 dir;
        Vec3 inv;
        Ray() {};
        Ray(Vec3 &_pos, Vec3 &_dir)
        {
            pos = _pos;
            dir = _dir;
            inv = Vec3(1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z);
        }
    };
}

#endif  // CBLT_RAY_H