#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>
#include <utility>
#include <algorithm>
#include "material.h"
#include "vec.h"

class Material;

/**
 * @brief A simple triangle structure. Used to store all the primitive geometry in the scene
 * 
 */
struct Triangle {
    //! The 3D locations of the triangle points
    Vec3 p1_;
    Vec3 p2_;
    Vec3 p3_;

    //! The 3D normals at each triangle point
    Vec3 n1_;
    Vec3 n2_;
    Vec3 n3_;

    //! The 3D tangents at each triangle point (used in texture mapping)
    Vec3 t1_;
    Vec3 t2_;
    Vec3 t3_;

    //! The 3D bitangents at each triangle point (used in texture mapping)
    Vec3 bt1_;
    Vec3 bt2_;
    Vec3 bt3_;

    //! The 2D UV coordinates at each triangle point (used in texture mapping)
    Vec2 uv1_;
    Vec2 uv2_;
    Vec2 uv3_;
    
    //! The triangle material
    Material *mat_;
};

/**
 * @brief A structure for describing all the information at a ray-scene collision point
 * 
 */
struct HitInfo {
    Vec3 pos;  //! The position of the collision
    Vec3 norm; //! The normal vector at the collision
    Vec2 uv;  //! The texture coordinates at the collision
    Material *m;  //! The surface material at the collision
    float t;  //! The ray time of the collision
    HitInfo() {
        t = INFINITY;
    }

    friend bool operator<(const HitInfo& lhs, const HitInfo& rhs) {
        return lhs.t < rhs.t;
    };
};

/**
 * @brief A enum for defining a light type. Currently, only 4 types are supported:
 * Point lights (infinitely small light sources)
 * Directional lights (infinitely large, and distant, light sources)
 * Spot lights (point lights, but they only emit light in certain angles)
 * Area lights (general emmiters, used for sampling soft shadows)
 */
enum LightType {POINT, DIR, SPOT, AREA};

/**
 * @brief A structure for describing a light object in a scene.
 * 
 */
struct Light {
    LightType type_;  //! The light type
    Color clr_;  //! The light color
    Vec4 src_;  //! The light positional/directional source - homogeneous coordinate determines whether it is a point or direction
    Vec3 dim_;  // x- constant, y-linear, z-quadratic, for spot and point lights
    Vec3 left_;  //! Light basis x vector - only applies to area lights
    Vec3 up_;  //! Light basis y vector - only applies to area lights
    // optional values
    float angle1_;  //! Innermost angle- only applies to spot lights
    float angle2_;  //! Outermost angle- only applies to spot lights
};

#endif  // STRUCTS_H