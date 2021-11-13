#include "ray_tracer.h"
#include "collision.h"
#include "random_sampler.h"
#include "sobol2D.h"
#define USE_MATH_DEFINES
#define M_PI 3.14159265f
#define USE_RAND
#undef USE_RAND
#include <cmath>
#include <ctime>
#include <chrono>
#include <omp.h>
#include <algorithm>

RayTracer::RayTracer(shared_ptr<Scene> scene_data) {
    image_scene_ = scene_data;
    max_depth_ = 8;
}

RayTracer::~RayTracer() {
}

shared_ptr<Image> RayTracer::Render(int width, int height) {
    shared_ptr<Image> result(new Image(width, height));
    float half_width = width * .5f;
    float half_height = height * .5f;
    int size = width * height;
    int num_samples = 1024;
    float frac = 1.f / static_cast<float>(num_samples);
    

    omp_set_num_threads(12);
    #pragma omp parallel
    {
        shared_ptr<Sampler2D> generator(new RandomSampler2D);
        #pragma omp for schedule(guided)
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {  
                Color tot_clr(0, 0, 0);
                for (int i = 1; i <= num_samples; i++) {
                    float jitter_x, jitter_y;
                    Color path_throughput(1.f, 1.f, 1.f);
                    generator->NextSample(jitter_x, jitter_y);
                
                    float u = half_width - (x + jitter_x);
                    float v = half_height - (y + jitter_y);

                    Ray ray = image_scene_->camera_.CreateRay(u, v);
                    tot_clr = tot_clr + PathTrace(ray, 0, generator, path_throughput) * frac;
                }
            //tot_clr = tot_clr / static_cast<float>(num_samples);
            result->setPixel(x, y, tot_clr);
            }
        }
    }
    return result;
}

Color RayTracer::PathTrace(Ray ray, int depth, shared_ptr<Sampler2D> generator, Color &path_throughput) {
    if(depth > max_depth_) {
        return Color(0,0,0);
    }
    // Check if we hit anything
    HitInfo hit_info;
    Color tot_color(0,0,0);
    Vec3 outgoing;
    bool hit = SceneIntersect(ray, hit_info);
    if(hit) {
        // there was a collision
        // right now we only check for diffuse in path tracing, but we 
        // may add specular, reflection, and transmissive later
        float pdf;
        Vec3 to_eye = image_scene_->camera_.Eye() - hit_info.pos;
        to_eye.Normalize();
        Color BSDF_clr = hit_info.m->Sample(ray.dir, outgoing, pdf, to_eye, hit_info, generator);
        Vec3 wiggle = hit_info.pos + .001f * outgoing;
        Ray recurse_ray(wiggle, outgoing);
        float cos_theta = hit_info.norm.Dot(outgoing);
        //cos_theta = 1.0f;
        Color indirect = PathTrace(recurse_ray, depth + 1, generator, path_throughput);
        tot_color = (hit_info.m->Emittance()) + BSDF_clr * indirect * cos_theta / pdf;
        //path_throughput = path_throughput * (std::abs(outgoing.Dot(hit_info.norm)) / pdf);
    }
    return tot_color;
}

// Check if the ray intersects the scene.
bool RayTracer::SceneIntersect(Ray ray, HitInfo &hit) {
    return image_scene_->scene_triangles_.intersect(ray, hit);
}