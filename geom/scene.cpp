#include "scene.h"

namespace cblt
{
    Scene::Scene(std::vector<std::shared_ptr<ScenePrim>> &prims)
    {
        prims_ = prims;
        accel_ = BoundingVolume<ScenePrim>(prims);
        // TODO - materials
    }

    void Scene::AddPrim(const std::shared_ptr<ScenePrim> &prim)
    {
        prims_.push_back(prim);
        // TODO - rebuild BVH?
    }

    bool Scene::Intersects(const Ray &ray)
    {
        HitInfo collision_pt;
        return accel_.Intersect(ray, collision_pt);
    }

    bool Scene::ClosestIntersection(const Ray &ray, HitInfo &collision_pt)
    {
        return accel_.Intersect(ray, collision_pt);
    }
}