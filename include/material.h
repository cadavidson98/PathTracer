#ifndef MATERIAL_H
#define MATERIAL_H


#include "image_lib.h"
#include "ray.h"
#include "sampler.h"
#include "structs.h"

#include <memory>

struct HitInfo;

/**
 * An interface for defining materials. This defines the essential functions for illumination and sampling
 * for both Ray Tracing and Path Tracing.
 */ 
class Material {
public:    
    // Ray Tracing Functions
    /**
     * \brief Illuminate this material using the Phong-Blinn illumination model
     * \param incoming Vector which defines the indicent light ray
     * \param outgoing Vector which defines the outgoing directions towards the camera eye
     * \param normal Vector which defines the surface normal at the collision point
     */ 
    virtual Color ApplyPhongBlinn(const Vec3 &incoming, const Vec3 &outgoing, const Vec3 &normal) = 0;
    // Path Tracing Functions
    /**
     * \brief Calculates the material color at the given location and viewing angle
     * \param incoming Vector which defines the incident light ray
     * \param collision Data struct containing the collision location and normal
     * \param outgoing Vector which defines the outgoing direction towards the camera eye
     */ 
    virtual Color Sample(const Vec3 &incoming, Vec3 &outgoing, float &pdf, const Vec3 &toEye, const HitInfo &collisionPt, std::shared_ptr<Sampler2D> BRDF_sampler) = 0;

    virtual Color Emittance() = 0;
};

#endif  // MATERIAL_H