#include "area_light.h"

#include <algorithm>

namespace cblt
{
    AreaLight::AreaLight(const Vec3 &pos, const Vec3 &X, const Vec3 &Y, const Vec3 &Z, 
                         const Color &clr, float energy, float length, float width) :
                         pos_(pos), dir_X_(X), dir_Y_(Y), dir_Z_(Z), 
                         color_(clr), length_(length), width_(width)
    {
        area_ = length_ * width_;
        power_ = energy / (PI_f * area_);
        light_material_ = std::make_shared<LambertianMaterial>(Color(1.f, 1.f, 1.f));
    }

    Color AreaLight::Sample(Vec3 &to_light, const Vec3 &surf_pos, const Vec3 &surf_norm, float &dist, float &pdf, std::shared_ptr<Sampler> &sampler)
    {
        float u, v;
        // samples return values in the range of [0, 1], so to center the area light 
        // at the origin, we need to shift the samples to [-.5, .5]
        sampler->Next2D(u, v);
        float x = length_ * (u - .5f);
        // y axis is up, which is the light direction
        float z = width_ * (v - .5f);
        Vec3 light_pos = pos_ + dir_X_ * x + dir_Z_ * z;
        // to light is un-normalized, 
        to_light = light_pos - surf_pos;
        float len_sqr = MagnitudeSqr(to_light);
        dist = std::sqrt(len_sqr);
        to_light = to_light / dist;
        pdf = 1.f / area_;
        return color_ * power_ * std::max(0.f, -Dot(to_light, dir_Y_)) / (len_sqr);
    }

    bool AreaLight::Intersect(const Ray &ray, HitInfo &collision_pt)
    {
        float dt = Dot(ray.dir, dir_Y_);
        if (std::abs(dt) < eps_zero_F)
        {
  	        // the ray is parallel, so return false
  	        return false;
  	    }

  	    float hit_t = Dot(pos_ - ray.pos, dir_Y_) / dt;
  	    
        if (hit_t < eps_zero_F)
        {
  	        // only go forward in the direction
  	        return false;
  	    }
	
  	    Vec3 hit_pos = ray.pos + ray.dir * hit_t;
        // use vector projection to check that our point
        // is in the quad
        Vec3 bot_left = pos_ - (dir_X_ * length_ + dir_Z_ * width_) * .5f;
        Vec3 to_hit_pos = hit_pos - bot_left;
        float proj_X = Dot(to_hit_pos, dir_X_);
        float proj_Z = Dot(to_hit_pos, dir_Z_);
        if (proj_X >= eps_zero_F && (proj_X - length_) <= eps_zero_F &&
            proj_Z >= eps_zero_F && (proj_Z - width_) <= eps_zero_F)
        {
            collision_pt.hit_time = hit_t;
            collision_pt.norm = dir_Y_;
            collision_pt.pos = hit_pos;
            collision_pt.emissive = true;
            collision_pt.emission = this;
            collision_pt.m = light_material_;
            return true;
        }
        return false;
    }

    BoundingBox AreaLight::GetBounds()
    {
        Vec3 min_pt(-.5f * length_, 0.f, -.5f * width_), 
             max_pt( .5f * length_, 0.f,  .5f * width_);
        return BoundingBox(min_pt, max_pt);
    }

    Color AreaLight::Emission()
    {
        return color_ * power_;
    }

    Color AreaLight::Radiance(const Vec3 &to_light, const Vec3 &surf_pos, const Vec3 &surf_norm, float &pdf)
    {
        // Area lighting Integral from Sebastian Legarde "Moving Frostbite to PBR 3.0"
        // We need to account for the distance attenuation when directly sampling the area light
        // Vec3 to_light = light_pos - surf_pos;
        float length_sqr = MagnitudeSqr(to_light);
        // Normalize
        Vec3 to_light_norm = to_light / std::sqrt(length_sqr);
        pdf = 1.f / area_;
        return color_ * power_ * AbsDot(to_light_norm, dir_Y_) / length_sqr;
    }
}