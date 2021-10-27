#include "ray_tracer.h"
#include "collision.h"

#define USE_MATH_DEFINES
#define M_PI 3.14
#define USE_RAND
#undef USE_RAND
#include <cmath>
#include <ctime>
#include <chrono>
#include <omp.h>
#include <algorithm>

RayTracer::RayTracer(shared_ptr<Scene> scene_data) {
    image_scene_ = scene_data;
    max_depth_ = 5;
}

RayTracer::~RayTracer() {
}

Image * RayTracer::RayTrace(int width, int height) {
    Image * result = new Image(width, height);
    float half_width = width * .5;
    float half_height = height * .5f;
    int size = width * height;
    Sobol2D generator;
    #pragma omp parallel for schedule(guided) firstprivate(generator)
    for(int y = 0; y < height; y++) {
      for(int x = 0; x < width; x++) {  
          Color tot_clr(0, 0, 0);
          int num_samples = 200;
          float frac = 1.f / static_cast<float>(num_samples);
          for (int i = 1; i <= num_samples; i++) {
              float jitter_x, jitter_y;
              #ifdef USE_RAND
                jitter_x = rand() / (float)RAND_MAX;
                jitter_y = rand() / (float)RAND_MAX;
              #else
                generator.SobolNext(jitter_x, jitter_y);
              #endif
              float u = half_width - (width * ((x + jitter_x) / width));
              float v = half_height - (height * ((y + jitter_y) / height));

              Ray ray = image_scene_->camera_.CreateRay(u, v);
              tot_clr = tot_clr + PathTrace(ray, 0, &generator, i) * frac;
          }
          result->setPixel(x, y, tot_clr);
          //printf("\r%.2f%%", (float)(y*width + x)/(width*height)*100.f);
      }
    }
    return result;
}

Color RayTracer::RayRecurse(Ray ray, int depth) {
    if(depth > max_depth_) {
        // It is black not background for a reason
        return Color(0,0,0);
    }
    // shoot ray from p in direction dir
    Color color = (depth == 0) ? Color(.5, .5, .5) : Color(0, 0, 0);
    HitInfo hit_info;
    bool hit = SceneIntersect(ray, hit_info);
    if(hit) {
        color = Color(0,0,0);
        Vec3 r_pt = hit_info.pos + .0001 * hit_info.norm;
        for(int i = 0; i < image_scene_->lights_.size(); i++) {
            color = color + LightPoint(*image_scene_->lights_[i], hit_info);
        }
        Color ambient = Color(.6, .6, .6) * (hit_info.m.a_);
        color = color + ambient;
        if (hit_info.m.diffuse_map_) {
            color = color * hit_info.m.diffuse_map_->sample(hit_info.uv.x, hit_info.uv.y);
        }

        // now we do the recursion
        // reflect
        if(hit_info.m.s_.r + hit_info.m.s_.g + hit_info.m.s_.b > 0.0) {
            Vec3 reflect = ray.dir - (2*hit_info.norm.Dot(ray.dir)*hit_info.norm);
            Ray reflect_ray(r_pt, reflect);
            Color ret = RayRecurse(reflect_ray, depth+1);
            color = color + (ret * hit_info.m.s_);
        }
        // refraction
        if(hit_info.m.t_.r + hit_info.m.t_.g + hit_info.m.t_.b > 0.0) {
            float dir = ray.dir.Dot(hit_info.norm);
            Vec3 r_in;
            if(dir > 0) {
                // leaving
                r_in = Refract(ray.dir.UnitVec(), -1 * hit_info.norm, hit_info.m.ior_, 1.0);
            }
            else if (dir < 0) {
                // entering
                r_in = Refract(ray.dir.UnitVec(), hit_info.norm, 1.0, hit_info.m.ior_);
            }
            // wiggle in
            Vec3 in_s = hit_info.pos + .005 * r_in;
            // find the out point
            // recurse!
            Ray refract_ray = (in_s, r_in);
            Color refrac_clr = RayRecurse(refract_ray, depth+1);
            Color amt = hit_info.m.t_;
            color = color + Color(refrac_clr.r*amt.r, refrac_clr.g*amt.g, refrac_clr.b*amt.b);
        }
    }
    return color;
}

Color RayTracer::PathTrace(Ray ray, int depth, Sobol2D* generator, int sample) {
  if(depth > max_depth_) {
    return Color(0,0,0);
  }
  // Check if we hit anything
  HitInfo hit_info;
  Color color(0,0,0);
  bool hit = SceneIntersect(ray, hit_info);
  if(hit) {
    Color diffuse_clr(0,0,0), mat_clr(0,0,0);
    Ray recurse_ray;
    // there was a collision
    // right now we only check for diffuse in path tracing, but we 
    // may add specular, reflection, and transmissive later
    hit_info.m.Sample(mat_clr);
    if (mat_clr == hit_info.m.d_) {
        Vec3 bitangent = hit_info.norm.Cross(ray.dir).UnitVec();
        Vec3 tangent = hit_info.norm.Cross(bitangent).UnitVec();

        Vec3 diffuse_dir = RandomUnitVectorInHemisphere(bitangent, hit_info.norm, tangent, generator, sample);
        // now compute BRDF
        Vec3 wiggle = (0.001f * diffuse_dir) + hit_info.pos;
        recurse_ray = Ray(wiggle, diffuse_dir);
        float cos_theta = diffuse_dir.Dot(hit_info.norm);
        //cos_theta = 1;
        Color BRDF = mat_clr * (1.0f / M_PI);
        Color incoming = PathTrace(recurse_ray, depth + 1, generator, sample);
        diffuse_clr = hit_info.m.e_ + BRDF * incoming * cos_theta * 2 * M_PI;
        if (hit_info.m.diffuse_map_ != NULL) {
            diffuse_clr = diffuse_clr * hit_info.m.diffuse_map_->sample(hit_info.uv.x, hit_info.uv.y);
        }
        color = diffuse_clr;
    }
    else if (mat_clr == hit_info.m.s_) {
        Vec3 reflect = ray.dir - (2 * hit_info.norm.Dot(ray.dir) * hit_info.norm);
        Vec3 r_pt = hit_info.pos + .0001f * reflect;
        Ray reflect_ray(r_pt, reflect);
        Color ret = PathTrace(reflect_ray, depth + 1, generator, sample);
        color = ret * mat_clr;
    }
    else if (mat_clr == hit_info.m.t_) {
        float dir = ray.dir.Dot(hit_info.norm);
        Vec3 r_in;
        if (dir > 0) {
            // leaving
            r_in = Refract(ray.dir.UnitVec(), -1 * hit_info.norm, hit_info.m.ior_, 1.0);
        }
        else if (dir < 0) {
            // entering
            r_in = Refract(ray.dir.UnitVec(), hit_info.norm, 1.0, hit_info.m.ior_);
        }
        // wiggle in
        Vec3 in_s = hit_info.pos + .005 * r_in;
        // find the out point
        // recurse!
        Ray refract_ray(in_s, r_in);
        Color refrac_clr = PathTrace(refract_ray, depth + 1, generator, sample);
        Color amt = hit_info.m.t_;
        color = refrac_clr * amt;
    }
  }
  return color;
}

/**
 * Create a random unit vector in the hemisphere specified by
 * pos and normal
 */ 
Vec3 RayTracer::RandomUnitVectorInHemisphere(Vec3 bitangent, Vec3 normal, Vec3 tangent, Sobol2D *generator, int sample) {
    // sart by getting a random unit vector in the unit (hemi)sphere
    float u1 = 0;
    float u2 = 0;
    //generator->GetNum(sample, u1, u2);
    #ifdef USE_RAND
    u1 = rand() / (float)RAND_MAX;
    u2 = rand() / (float)RAND_MAX;
    #else
    // calculates 2 quasi-random numbers in the range 0-1 inclusive
    generator->SobolNext(u1, u2);
    #endif
    float radius = sqrtf(1.f - u1 * u1);
    float theta = 2.f * M_PI * u2;
    
    float x = radius * cos(theta);
    float z = radius * sin(theta);
    
    Vec3 rand_vec(x, u1, z);

    //Vec3 normal_in_hemi = Vec3(x, y, z).UnitVec();
    //Vec3 tangent = Vec3(normal.z, 0, -normal.x).UnitVec();
    //Vec3 bitangent = normal.Cross(tangent).UnitVec();
    //float theta = rand()/(float)RAND_MAX * 2 * M_PI;
    //float phi = rand()/(float)RAND_MAX * M_PI;
    //Vec3 rand_vec(cos(theta), cos(phi), sin(theta));
    Vec3 result;
    Matrix4x4 BNT(Vec4(bitangent, 0), Vec4(normal, 0), Vec4(tangent, 0), Vec4(0, 0, 0, 1));
    Vec4 out_vec = BNT * Vec4(rand_vec, 0);

    result.x = out_vec.x;
    result.y = out_vec.y;
    result.z = out_vec.z;
  
    return result.UnitVec();
}

Vec3 RayTracer::Refract(Vec3 d, Vec3 n, float n_i, float n_r) {
  float m = n_i / n_r;
  float dot_prod = d.Dot(n);
  Vec3 t = m * (d - n*dot_prod) - n*sqrt(1.0 - m*m*(1.0 - dot_prod*dot_prod));
  return t.UnitVec();
}

// Apply lighting to the point P using Phong Illumination,
// as well as reflection and refraction
Color RayTracer::LightPoint(Light l, HitInfo pt) {
    Color clr(0, 0, 0);
    
    if (pt.m.diffuse_map_) {
        clr = clr * pt.m.diffuse_map_->sample(pt.uv.x, pt.uv.y);
    }
    // get ambient light
    float time;
    float i = 1.0;  
    Vec3 to_light;
    Vec3 to_eye;
    // get light vectors
    switch(l.type_) {
        case point: {
            to_light = l.pos_ - pt.pos;
            to_eye = image_scene_->camera_.Eye() - pt.pos;
            i = 1.0 / (to_light.MagnitudeSqr() + .0001);
            time = to_light.MagnitudeSqr();
            break;
        }
        case dir: {
            to_light = (l.dir1_ * -1.0).UnitVec();
            to_eye = image_scene_->camera_.Eye() - pt.pos;
            time = INFINITY;
            break;
        }
        case spot: {
            // get the angle from the light and point
            to_light = l.pos_ - pt.pos;
            time = to_light.MagnitudeSqr();
            float angle = std::acosf(-1*to_light.UnitVec().Dot((l.dir1_).UnitVec()));
            if(angle < l.angle1_) {
                // behave like point light
                to_eye = image_scene_->camera_.Eye() - pt.pos;
                i = 1.0 / (to_light.MagnitudeSqr() + .0001);
            }
            else if(angle > l.angle2_) {
                to_eye = image_scene_->camera_.Eye() - pt.pos;
                i = 0.0;
            }
            else {
                // linear falloff
                to_eye = image_scene_->camera_.Eye() - pt.pos;
                i = ((l.angle2_ - angle) / (l.angle2_ - l.angle1_)) * (1.0 / (to_light.MagnitudeSqr() + .0001));
            }
        }
    }

    // now check if we are in shadow
    HitInfo shadow_hit;
    Vec3 wiggle = pt.pos + to_light.UnitVec()*.1;
    Ray shadow_ray(wiggle, to_light.UnitVec());
    bool in_shadow = SceneIntersect(shadow_ray, shadow_hit);
    
    if(in_shadow && (shadow_hit.pos - pt.pos).MagnitudeSqr() < time) {
        // we are in the shadow AND we hit an object
        // before reaching the light
        return clr;
    }
    else {
        float dir = pt.norm.Dot(to_light);
        Vec3 norm = pt.norm;
        Vec3 r  = (2 * to_light.Dot(norm) * norm - to_light).UnitVec();
        float kd = i * std::max((float)0.0, norm.Dot(to_light.UnitVec()));
        float ks = std::max((float)0.0, pow(r.Dot(to_eye.UnitVec()), pt.m.ns_));
        
        Color diffuse = l.clr_* (pt.m.d_ * kd);
        Color specular = pt.m.s_ * ks;
        clr = clr + diffuse + specular;
    }
    return clr;
}

bool RayTracer::SceneIntersectFast(Ray ray) {
  HitInfo temp;
  return image_scene_->scene_triangles_.intersect(ray, temp);
}

// Check if the ray intersects the scene.
bool RayTracer::SceneIntersect(Ray ray, HitInfo &hit) {
  return image_scene_->scene_triangles_.intersect(ray, hit);
}