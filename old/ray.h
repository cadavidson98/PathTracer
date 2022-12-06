#ifndef RAY_H
#define RAY_H

#include "vec.h"

/**
 * @brief A simple structure for defining a ray with a position and direction.
 * Used to render ray tracing and path tracing scenes.
 */
struct Ray {
    public:
        Ray(Vec3 _pos = Vec3(), Vec3 _dir = Vec3()) : inverse(Vec3()) {
            pos = _pos;
            dir = _dir;
            inverse = 1.0f / dir;
            dir.Normalize();
            inverse.Normalize();
        };
        Vec3 pos;  //! Ray starting position
        Vec3 dir;  //! Ray direction
        Vec3 inverse;  //! 1 / dir, used to eliminate redundant division in ray-scene intersections
};

#endif  // RAY_H