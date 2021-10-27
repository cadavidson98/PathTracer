#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>
#include <utility>
#include <algorithm>
#include "image_lib.h"
#include "vec.h"

using namespace std;

struct Material {
    Color a_;   // ambient
    Color d_;   // diffuse
    Color s_;   // specular
    Color t_;   // transmissive
    Color e_;   // emissive
    float ns_;  // cosine power thingy
    float ior_; // index of refraction
    Image* normal_map_ = NULL;
    Image* diffuse_map_ = NULL;
    // Default is a matte white
    Material(Color a=Color(0,0,0), Color d=Color(1,1,1), Color s=Color(0,0,0), 
            float ns=5, Color t=Color(0,0,0), float ior=1, Color e=Color(0,0,0)) : a_(a),
            d_(d), s_(s), ns_(ns), t_(t), ior_(ior), e_(e) {};
    void Sample(Color& mat_clr) {
        // Get luminance of each color
        float diff_lum = d_.r * .76f + d_.g * 1.5f + d_.b * .29f;
        float spec_lum = s_.r * .76f + s_.g * 1.5f + s_.b * .29f;
        float trans_lum = t_.r * .76f + t_.g * 1.5f + t_.b * .29f;
        float tot_lum = diff_lum + spec_lum + trans_lum;
        // now get the probability of each component
        float use_diff = diff_lum / tot_lum;
        float use_spec = spec_lum / tot_lum;
        float use_tran = trans_lum / tot_lum;
        // generate a random number
        float rand_clr = rand() / (float)RAND_MAX;
        if (rand_clr < use_diff) {
            // This sample will be diffusive
            mat_clr = d_;
        }
        else if( rand_clr < use_diff + use_spec) {
            // This sample will be reflective
            mat_clr = s_;
        }
        else if (rand_clr < use_diff + use_spec + use_tran) {
            // This sample will be refractive
            mat_clr = t_;
        }
    }
};

struct Sphere {
    Vec3 center_;
    float radius_;
    Material mat_;
    Sphere(Vec3 c=Vec3(0,0,0), float r=0.0) : center_(c), radius_(r) {};
};

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
    // pointer to hopefully offset massive memory
    // requirements
    Material *mat_;
};

struct HitInfo {
    Vec3 pos;
    Vec3 norm;
    Vec2 uv;
    Material m;
    float t;
    HitInfo() {
        t = INFINITY;
    }
    HitInfo(const HitInfo &other) {
        pos = other.pos;
        norm = other.norm;
        uv = other.uv;
        m = other.m;
        t = other.t;
    };

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