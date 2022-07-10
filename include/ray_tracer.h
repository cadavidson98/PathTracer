#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "image_lib.h"

#include "structs.h"
#include "scene.h"
#include "bvh.h"
#include "sampler.h"

#include <string>
#include <vector>
#include <memory>

using namespace std;

class RayTracer {
public:
    RayTracer(shared_ptr<Scene> image_scene_);
    ~RayTracer();
    shared_ptr<Image> Render(int width, int height);
private:
    int max_depth_;
    int num_samples_;    
    shared_ptr<Scene> image_scene_;

    Color PathTrace(Ray ray, int depth, shared_ptr<Sampler2D> generator, Color &path_throughput);
    Color PathTraceIterative(Ray cam_ray, shared_ptr<Sampler2D> generator);
    Color RayCast(Ray ray, shared_ptr<Sampler2D> generator);
    Color SampleLights(const Ray &incoming, const HitInfo &surface, shared_ptr<Sampler2D> generator);
    bool SceneIntersect(Ray ray, HitInfo &hit);
};

#endif  // RAYTRACER_H