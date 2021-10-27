#ifndef RAY_H
#define RAY_H

#include "vec.h"

class Ray {
    public:
        Ray(Vec3 _pos = Vec3(), Vec3 _dir = Vec3()) : inverse(Vec3()) {
            pos = _pos;
            dir = _dir;
            inverse = 1.0f / dir;
            dir.Normalize();
            inverse.Normalize();
        };
        Vec3 pos;
        Vec3 dir;
        Vec3 inverse;
    private:

};

#endif  // RAY_H