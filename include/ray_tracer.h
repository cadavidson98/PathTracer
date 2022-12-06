#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "image_lib.h"

#include "geom/scene.h"
#include "sampler.h"

#include <string>
#include <vector>
#include <memory>

using namespace std;

class RayTracer {
public:
    RayTracer(shared_ptr<cblt::Scene> &image_scene_);
    ~RayTracer();
    shared_ptr<Image> Render(int width, int height);
private:
    int max_depth_;
    int num_samples_;    
    shared_ptr<cblt::Scene> image_scene_;

    //Color PathTrace(cblt::Ray ray, int depth, shared_ptr<Sampler2D> generator, Color &path_throughput);
    Color PathTraceIterative(cblt::Ray cam_ray, shared_ptr<cblt::Sampler2D> &generator);
    //Color RayCast(cblt::Ray ray, shared_ptr<Sampler2D> generator);
    //Color SampleLights(const cblt::Ray &incoming, const cblt::HitInfo &surface, shared_ptr<Sampler2D> generator);
    bool SceneIntersect(cblt::Ray ray, cblt::HitInfo &hit);
};

#endif  // RAYTRACER_H