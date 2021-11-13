#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>
#include <utility>
#include <algorithm>
#include "material.h"
#include "vec.h"

class Material;

struct Triangle {
    Vec3 p1_;
    Vec3 p2_;
    Vec3 p3_;

    Vec3 n1_;
    Vec3 n2_;
    Vec3 n3_;

    Vec3 t1_;
    Vec3 t2_;
    Vec3 t3_;

    Vec3 bt1_;
    Vec3 bt2_;
    Vec3 bt3_;

    Vec2 uv1_;
    Vec2 uv2_;
    Vec2 uv3_;
    
    Material *mat_;
};

struct HitInfo {
    Vec3 pos;
    Vec3 norm;
    Vec2 uv;
    Material *m;
    float t;
    HitInfo() {
        t = INFINITY;
    }

    friend bool operator<(const HitInfo& lhs, const HitInfo& rhs) {
        return lhs.t < rhs.t;
    };
};

enum LightType {point, dir, spot, emissive};

struct Light {
    LightType type_;
    Color clr_;
    Color a_;
    Color s_;
    // optional values
    Vec3 pos_;
    Vec3 dir1_;
    float angle1_;
    float angle2_;
    float attenu_const_;
    float attenu_lin_;
    float attenu_quad_;
};

#endif  // STRUCTS_H