#include "ray_tracer.h"

#include <cmath>
#include <algorithm>
#include <omp.h>

#include "math/math_helpers.h"
#include "math/constants.h"

#include "mat/material.h"
#include "mat/random_sampler.h"
#include "mat/sobol2D.h"

RayTracer::RayTracer(shared_ptr<cblt::Scene> &scene_data) {
    image_scene_ = scene_data;
    max_depth_ = 8;
    num_samples_ = 128;
}

RayTracer::~RayTracer() {
}

shared_ptr<Image> RayTracer::Render(int width, int height) {
    shared_ptr<Image> result = std::make_shared<Image>(width, height);
    float half_width = width * .5f;
    float half_height = height * .5f;
    int size = width * height;
    
    float frac = 1.f / static_cast<float>(num_samples_);
    
    omp_set_num_threads(10);
    #ifndef _DEBUG
    #pragma omp parallel
    #endif
    {
        shared_ptr<cblt::Sampler> generator = std::make_shared<cblt::RandomSampler>(omp_get_thread_num() * 1234U);
        #ifndef _DEBUG
        #pragma omp for schedule(guided)
        #endif
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {  
                Color tot_clr(0, 0, 0);
                for (int i = 1; i <= num_samples_; i++) {
                    float jitter_x, jitter_y;
                    generator->Next2D(jitter_x, jitter_y);
                
                    float u = half_width - (x + jitter_x);
                    float v = half_height - (y + jitter_y);

                    cblt::Ray ray = image_scene_->cam_.CreateRay(u, v);
                    tot_clr = tot_clr + PathTraceIterative(ray, generator);
                }
                tot_clr = tot_clr * frac;
                // Reihard Tone Map
                Color hdr = tot_clr / (Color(1.f, 1.f, 1.f) + tot_clr);
                result->setPixel(x, y, Color(std::pow(hdr.r, 1.f/2.2f), std::pow(hdr.g, 1.f/2.2f), std::pow(hdr.b, 1.f/2.2f)));
                //result->setPixel(x, y, tot_clr);
            }
        }
    }
    return result;
}
/*
Color RayTracer::RayCast(cblt::Ray ray, std::shared_ptr<Sampler2D> generator) {
    cblt::HitInfo scenePt;
    bool hit = SceneIntersect(ray, scenePt);
    int samples_per_light = 1;
    Color illum(0.f, 0.f, 0.f);
    if (hit) {
        for (Light* light : image_scene_->lights_) {
            Color contrib(0.f, 0.f, 0.f);
            for(int i = 0; i < samples_per_light; ++i) {
                Vec3 to_light;
                switch (light->type_) {
                    case DIR: {
                        to_light = Vec3(-light->src_.x, -light->src_.y, -light->src_.z);
                        samples_per_light = 1;
                        break;
                    }
                    case AREA: {
                        float u, v;
                        generator->NextSample(u, v);
                        // map [0, 1] to [-w/2, w/2]
                        u = light->dim_.x * (u - 0.5f);
                        v = light->dim_.y * (v - 0.5f);
                        Vec4 light_pos = light->src_ + Vec4(light->left_ * u, 0) + Vec4(light->up_ * v, 0);
                        to_light = Vec3(light_pos.x, light_pos.y, light_pos.z) - scenePt.pos;
                        samples_per_light = 16;
                        break;
                    }
                    default: {
                        to_light = Vec3(light->src_.x, light->src_.y, light->src_.z) - scenePt.pos;
                        samples_per_light = 1;
                        break;
                    }
                }
                Vec3 to_eye = Vec3(0,0,0) - ray.dir;
                to_eye.Normalize();
                float dist = to_light.MagnitudeSqr();
                to_light.Normalize();
                Ray shadow_ray(scenePt.pos + to_light * .001f, to_light);
                HitInfo occulder;
                bool in_shadow = SceneIntersect(shadow_ray, occulder);
                if (!in_shadow || occulder.t * occulder.t > dist) {
                    contrib = contrib + scenePt.m->BRDF(to_eye, to_light, scenePt) * to_light.Dot(scenePt.norm);
                }
            }
            contrib = contrib / static_cast<float>(samples_per_light);
            illum = illum + contrib;
        }
    }
    return illum;
}

Color RayTracer::PathTrace(Ray ray, int depth, shared_ptr<Sampler2D> generator, Color &path_throughput) {
    // Check if we hit anything
    HitInfo hit_info;
    Color tot_color(0,0,0);
    Vec3 outgoing;
    bool hit = SceneIntersect(ray, hit_info);
    if (hit) {
        //Color direct = SampleLights(ray, hit_info, generator);
        if(depth >= max_depth_) {
            return tot_color;
            //return direct;
        }
        // there was a collision
        // right now we only check for diffuse and specular, still need to
        // add transmissive and the frenel effect
        float pdf;
        Vec3 to_eye = image_scene_->camera_.Eye() - hit_info.pos;
        to_eye.Normalize();
        Color BRDF_clr = hit_info.m->Sample(ray.dir, outgoing, pdf, hit_info, generator);
        // Check if the BRDF scatters light
        if (BRDF_clr.r + BRDF_clr.g + BRDF_clr.b > .0001f) {
            Vec3 wiggle = hit_info.pos + .001f * outgoing;
            Vec3 incoming = ray.dir * -1.f;
            incoming.Normalize();
            Ray recurse_ray(wiggle, outgoing);
            float cos_theta = hit_info.norm.Dot(outgoing);
            Color indirect = PathTrace(recurse_ray, depth + 1, generator, path_throughput);
            tot_color = BRDF_clr * cos_theta * indirect / pdf;
            tot_color = tot_color + hit_info.m->Emittance();
        }
    }
    return tot_color;
}
*/
Color RayTracer::PathTraceIterative(cblt::Ray cam_ray, shared_ptr<cblt::Sampler> &generator)
{
    Color tot_light(0.f, 0.f, 0.f), throughput(1.f, 1.f, 1.f);
    cblt::Ray path_ray = cam_ray;
    for (int depth = 0; depth < max_depth_; ++depth) {
        // intersect scene
        cblt::HitInfo scene_pt;
        scene_pt.hit_time = cblt::inf_F;
        bool hit = SceneIntersect(path_ray, scene_pt);
        
        if (!hit) 
        {
            break;
        }

        if (depth == 0)
        {
            // get emission for area lights directly visible to camera
            tot_light = tot_light + throughput * scene_pt.Emittance();
        }
        
        // compute direct lighting contribution
        tot_light = tot_light + throughput * image_scene_->SampleSingleLight(-path_ray.dir, scene_pt, generator);
        
        // sample BRDF at point to determine how light is transmitted to the next point on the path
        cblt::Vec3 incoming;
        float pdf;
        Color f = scene_pt.m->Sample(-path_ray.dir, incoming, pdf, scene_pt, generator);
        if (f.r + f.g + f.b < cblt::eps_zero_F)
        {
            break;
        }
        
        float cos_theta = cblt::AbsDot(scene_pt.norm, incoming);
        throughput = throughput * f * cos_theta / pdf;
        
        path_ray = cblt::Ray(scene_pt.pos + incoming * cblt::eps_zero_F, incoming);

        // russian roulette
        if (depth > 3)
        {
            float p = throughput.Luminance();
            float cutoff;
            generator->Next1D(cutoff);
            if(cutoff > p)
            {
                break;
            }
            throughput = throughput / p;
        }
    }
    return tot_light;
}
/*
Color RayTracer::SampleLights(const Ray &incoming, const HitInfo &surface, shared_ptr<Sampler2D> generator) {
    int idx = rand() % static_cast<int>(image_scene_->lights_.size());
    Light *light = image_scene_->lights_[idx];
    if (light->type_ == LightType::DIR) {
        // only directional right now for fast testing/experimentation
        Vec3 to_light(-light->src_.x, -light->src_.y, -light->src_.z);
        to_light.Normalize();
        Ray light_ray(surface.pos + (to_light * .001f), to_light);
        HitInfo occuluder_info;
        bool hit_occuluder = SceneIntersect(light_ray, occuluder_info);
        Color surf_clr = surface.m->BRDF(incoming.dir, to_light, surface);
        return (false && hit_occuluder) ? Color(0,0,0) : surf_clr * light->clr_;
    }
    else if (light->type_ == LightType::AREA) {
        float u, v;
        generator->NextSample(u, v);
        // map [0, 1] to [-w/2, w/2]
        u = light->dim_.x * (u - 0.5f);
        v = light->dim_.y * (v - 0.5f);
        // find the point in world space to shoot a shadow ray to
        Vec4 light_pos = light->src_ + Vec4(light->left_ * u, 0) + Vec4(light->up_ * v, 0);
        Vec3 to_light = Vec3(light_pos.x, light_pos.y, light_pos.z) - surface.pos;
        float dist = to_light.MagnitudeSqr();
        to_light.Normalize();
        Ray shadow_ray(surface.pos + to_light * .001f, to_light);
        HitInfo occuluder_info;
        bool hit_occuluder = SceneIntersect(shadow_ray, occuluder_info);
        Color surf_clr = surface.m->BRDF(incoming.dir, to_light, surface);
        return (hit_occuluder && occuluder_info.t * occuluder_info.t < dist) ? Color(0, 0, 0) : surf_clr * light->clr_;
    }
    else {
        return Color(0,0,0);
    }
}
*/
// Check if the ray intersects the scene.
bool RayTracer::SceneIntersect(cblt::Ray ray, cblt::HitInfo &hit) {
    return image_scene_->ClosestIntersection(ray, hit);
}