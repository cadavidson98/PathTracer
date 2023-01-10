#ifndef CBLT_SCENE_H
#define CBLT_SCENE_H

#include "scene_prim.h"
#include "bounding_volume.h"
#include "camera.h"

#include "light/light.h"


#include <vector>
#include <memory>

namespace cblt
{
    class Scene
    {
        public:
        Scene(const Camera & camera, std::vector<std::shared_ptr<ScenePrim>> &s_prims, std::vector<std::shared_ptr<Light>> &l_prims);
        void AddPrim(const std::shared_ptr<ScenePrim> &prim);
        void AddPrim(const std::shared_ptr<Light> &prim);
        bool Intersects(const Ray &ray, float &time);
        bool ClosestIntersection(const Ray &ray, HitInfo &collision_pt);
        Color SampleSingleLight(const Vec3 &outgoing, const HitInfo &collision_pt, std::shared_ptr<Sampler> &sampler);
        Color DirectLight(const Vec3 &outgoing, std::shared_ptr<Light> &light, const HitInfo &collision_pt, std::shared_ptr<Sampler> &sampler);
        Camera cam_;
        private:
        std::vector<std::shared_ptr<Light>> l_prims_;
        std::vector<std::shared_ptr<ScenePrim>> s_prims_;
        BoundingVolume<ScenePrim> accel_;
    };
}

#endif  // CBLT_SCENE_H