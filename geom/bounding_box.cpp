#include "bounding_box.h"

namespace cblt {
    
    BoundingBox::BoundingBox(const Vec3 &mn, const Vec3 &mx) :
        min_(mn), max_(mx)
    {
        CalculateCenter();
    }

    void BoundingBox::CalculateCenter() {
        cen_ = (max_ + min_) * .5f;
    }

    float BoundingBox::SurfaceArea() const {
        return  2.f * (
                ((max_.x - min_.x) * (max_.y - min_.y)) +
                ((max_.x - min_.x) * (max_.z - min_.z)) +
                ((max_.z - min_.z) * (max_.y - min_.y))
                );
    }

    BoundingBox BoundingBox::Union(const BoundingBox &other) const {
        BoundingBox uni;
        uni.min_.x = std::min(min_.x, other.min_.x);
        uni.min_.y = std::min(min_.y, other.min_.y);
        uni.min_.z = std::min(min_.z, other.min_.z);

        uni.max_.x = std::max(max_.x, other.max_.x);
        uni.max_.y = std::max(max_.y, other.max_.y);
        uni.max_.z = std::max(max_.z, other.max_.z);

        return uni;
    }

    bool BoundingBox::Intersect(const Ray &ray, float &intersect_time)
    {
        float tmin(-inf_F), tmax(inf_F);
        float tx1 = (min_.x - ray.pos.x) * ray.inv.x;
        float tx2 = (max_.x - ray.pos.x) * ray.inv.x;
    
	    tmin = std::max(tmin, std::min(tx1, tx2));
        tmax = std::min(tmax, std::max(tx1, tx2));
    
	    float ty1 = (min_.y - ray.pos.y) * ray.inv.y;
        float ty2 = (max_.y - ray.pos.y) * ray.inv.y;
    
	    tmin = std::max(tmin, std::min(ty1, ty2));
        tmax = std::min(tmax, std::max(ty1, ty2));
    
	    float tz1 = (min_.z - ray.pos.z) * ray.inv.z;
        float tz2 = (max_.z - ray.pos.z) * ray.inv.z;
    
	    tmin = std::max(tmin, std::min(tz1, tz2));
        tmax = std::min(tmax, std::max(tz1, tz2));
    
	    if(tmax > 0 && tmax >= tmin)
        {
            intersect_time = (tmin >= 0) ? tmin : tmax;
            return true;
        }
        return false;
    }
}