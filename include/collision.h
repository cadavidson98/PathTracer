#ifndef COLLISION_H
#define COLLISION_H

#include "structs.h"
#include "bvh.h"
#include "matrix.h"
#include <algorithm>

inline bool TriangleIntersect(const Ray &ray, const Triangle &tri, HitInfo &hit) {
  	// Get the plane normal
  	Vec3 vec1 = tri.p3_ - tri.p1_;
  	Vec3 vec2 = tri.p2_ - tri.p1_;
  	Vec3 norm = vec1.Cross(vec2);
  	float tri_mag = .5f * norm.Magnitude();
  	norm.Normalize();
  	float dt = fabs(ray.dir.Dot(norm));

  	if(dt < .001f) {
  	  // the ray is parallel, so return false
  	  return false;
  	}

  	float denom = ray.dir.Dot(norm);
  	float hit_t = (tri.p1_ - ray.pos).Dot(norm) / denom;
  	if(hit_t < 0) {
  	  // only go forward in the direction
  	  return false;
  	}
	
  	Vec3 hit_pos = ray.pos + ray.dir * hit_t;
  	// now check if it is inside the triangle using barycentric coordinates
  	float a, b, c;
	
  	Vec3 to_p3 = tri.p3_ - hit_pos;
  	Vec3 to_p2 = tri.p2_ - hit_pos;
  	Vec3 to_p1 = tri.p1_ - hit_pos;

  	a = .5f * to_p3.Cross(to_p2).Magnitude() / tri_mag;
  	b = .5f * to_p3.Cross(to_p1).Magnitude() / tri_mag;
  	c = .5f * to_p1.Cross(to_p2).Magnitude() / tri_mag;

  	if(a <= 1.0001f && b <= 1.0001f && c <= 1.0001f && a + b + c <= 1.0001f) {
    	hit.pos = hit_pos;
    	hit.t = hit_t;
    	
    	hit.norm = a * (tri.n1_) + b * (tri.n2_) + c * (tri.n3_);
    	
    	hit.m = tri.mat_;
    	hit.uv = a * (tri.uv1_) + b * (tri.uv2_) + c * (tri.uv3_);
    	if (tri.mat_->normal_map_ != nullptr) {
    	    // change of basis
    	    Vec3 tangent = a * (tri.t1_) + b * (tri.t2_) + c * (tri.t3_);
    	    Vec3 bitangent = a * (tri.bt1_) + b * (tri.bt2_) + c * (tri.bt3_);
    	    Vec3 normal = a * (tri.n1_) + b * (tri.n2_) + c * (tri.n3_);
    	    Matrix4x4 TBN(Vec4(tangent, 0), Vec4(bitangent, 0), Vec4(normal, 0), Vec4(0, 0, 0, 1));
    	    // Get normal from texture
    	    Color norm_clr = tri.mat_->normal_map_->sample(hit.uv.x, hit.uv.y);
    	    Vec4 tex_norm = 2.0f * Vec4(norm_clr.r, norm_clr.g, norm_clr.b) - 1.0f;
    	    tex_norm = TBN * tex_norm;
    	    hit.norm = Vec3(tex_norm.x, tex_norm.y, tex_norm.z);
    	}
		hit.norm.Normalize();
    	hit.norm = (hit.norm.Dot(ray.dir) > 0.0) ? -1*hit.norm: hit.norm;
		return true;
  	}
  	return false;
}

inline bool AABBIntersect(const Ray &ray, const Dimension &AABB, float &i_t) {
    float tmin(-INFINITY), tmax(INFINITY);
    float tx1 = (AABB.min_x_ - ray.pos.x) * ray.inverse.x;
    float tx2 = (AABB.max_x_ - ray.pos.x) * ray.inverse.x;
    
	tmin = std::max(tmin, std::min(tx1, tx2));
    tmax = std::min(tmax, std::max(tx1, tx2));
    
	float ty1 = (AABB.min_y_ - ray.pos.y) * ray.inverse.y;
    float ty2 = (AABB.max_y_ - ray.pos.y) * ray.inverse.y;
    
	tmin = std::max(tmin, std::min(ty1, ty2));
    tmax = std::min(tmax, std::max(ty1, ty2));
    
	float tz1 = (AABB.min_z_ - ray.pos.z) * ray.inverse.z;
    float tz2 = (AABB.max_z_ - ray.pos.z) * ray.inverse.z;
    
	tmin = std::max(tmin, std::min(tz1, tz2));
    tmax = std::min(tmax, std::max(tz1, tz2));
    
	if(tmax > 0 && tmax >= tmin) {
        i_t = (tmin >= 0) ? tmin : tmax;
        return true;
    }
    return false;
}
#endif  // COLLISION_H