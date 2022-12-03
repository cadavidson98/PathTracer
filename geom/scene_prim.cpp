#include "scene_prim.h"

namespace cblt
{
    ScenePrim::ScenePrim(const std::shared_ptr<Geometry> &model, const Mat4 &transform /*Material*/)
    {
        model_ = model;
        local_to_world_ = transform;
        world_to_local_ = OrthoInverse(transform);
    }

    BoundingBox ScenePrim::GetBounds()
    {
        BoundingBox local_bnds = model_->GetBounds();
        Vec4 world_min = world_to_local_ * Vec4(local_bnds.min_, 1.0f);
        Vec4 world_max = world_to_local_ * Vec4(local_bnds.max_, 1.0f);
        local_bnds.min_ = Vec3(world_min.x, world_min.y, world_min.z);
        local_bnds.max_ = Vec3(world_max.x, world_max.y, world_max.z);
        local_bnds.CalculateCenter();
        return local_bnds;
    }

    Ray ScenePrim::TransformRay(const Ray &world_ray)
    {
        Ray local_ray;
        Vec4 loc_dir = local_to_world_ * Vec4(world_ray.dir, 0.f);
        Vec4 loc_pos = local_to_world_ * Vec4(world_ray.pos, 1.f);
        local_ray.dir = Vec3(loc_dir.x, loc_dir.y, loc_dir.z);
        local_ray.pos = Vec3(loc_pos.x, loc_pos.y, loc_pos.z);
        return local_ray;
    }

    bool ScenePrim::Intersect(const Ray &ray, HitInfo &collision_pt)
    {
        // transform to local reference frame
        Ray local_ray = TransformRay(ray);
        // intersect againt the model in its local reference frame
        HitInfo local_hit;
        bool hit = model_->Intersect(local_ray, local_hit);
        if (hit)
        {
            // transform the hit info back into world space
            Vec4 world_pos = local_to_world_ * Vec4(local_hit.pos, 1.f);
            Vec4 world_norm = local_to_world_ * Vec4(local_hit.norm, 0.f);
            collision_pt.pos = Vec3(world_pos.x, world_pos.y, world_pos.z);
            collision_pt.norm = Vec3(world_norm.x, world_norm.y, world_norm.z);
            collision_pt.shading_basis = local_hit.shading_basis * world_to_local_;
            collision_pt.hit_time = Magnitude(ray.pos - collision_pt.pos);
            return true;
        }
        else
        {
            return false;
        }
    }
}