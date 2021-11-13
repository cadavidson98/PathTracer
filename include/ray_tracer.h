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
    
    shared_ptr<Scene> image_scene_;

    Color PathTrace(Ray ray, int depth, shared_ptr<Sampler2D> generator, Color &path_throughput);
    
    bool SceneIntersect(Ray ray, HitInfo &hit);
};

#endif  // RAYTRACER_H