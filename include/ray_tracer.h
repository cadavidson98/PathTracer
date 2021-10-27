#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "image_lib.h"

#include "structs.h"
#include "scene.h"
#include "bvh.h"
#include "sobol2D.h"

#include <string>
#include <vector>
#include <memory>

using namespace std;

class RayTracer {
public:
    RayTracer(shared_ptr<Scene> image_scene_);
    ~RayTracer();
    Image* RayTrace(int width, int height);
private:
    // This is where the result is drawn to
    int width_;
    int height_;
    int max_depth_;
    
    shared_ptr<Scene> image_scene_;
    Sobol2D sobol_generator;

    Color RayRecurse(Ray ray, int depth);
    Color PathTrace(Ray ray, int depth, Sobol2D* generator, int sample);
    Color LightPoint(Light l, HitInfo pt);
    Vec3 RandomUnitVectorInHemisphere(Vec3 bitangent, Vec3 norm, Vec3 tangent, Sobol2D* generator, int sample);
    bool SceneIntersect(Ray ray, HitInfo &hit);
    bool SceneIntersectFast(Ray ray);
    Vec3 Refract(Vec3 d, Vec3 n, float n_i, float n_r);
};

#endif  // RAYTRACER_H