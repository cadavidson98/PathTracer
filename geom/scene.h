#ifndef CBLT_SCENE_H
#define CBLT_SCENE_H

#include "scene_prim.h"
#include "bounding_volume.h"
#include "camera.h"

#include <vector>
#include <memory>

namespace cblt
{
    class Scene
    {
        public:
        Scene(const Camera & camera, std::vector<std::shared_ptr<ScenePrim>> &prims);
        void AddPrim(const std::shared_ptr<ScenePrim> &prim);
        bool Intersects(const Ray &ray);
        bool ClosestIntersection(const Ray &ray, HitInfo &collision_pt);
        
        Camera cam_;
        private:
        std::vector<std::shared_ptr<ScenePrim>> prims_;
        BoundingVolume<ScenePrim> accel_;

        
    };
}

#endif  // CBLT_SCENE_H