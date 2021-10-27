#ifndef CAMERA_H
#define CAMERA_H

#include "vec.h"
#include "ray.h"

enum camera_type {
    orthographic,
    perspective,
    cabinet,
    cavalier
};

class Camera {
    public:
        Camera(Vec3 eye = Vec3(0,0,0), Vec3 forward = Vec3(0,0,-1), Vec3 up = Vec3(0,1,0), float horiz_half_fov = 3.14f/4.f, camera_type proj_type = camera_type::perspective);
        void SetDist(float near_dist);
        float GetHalfFOV() { return half_angle_fov_; };
        Vec3 Eye() { return cam_eye_; };
        Ray CreateRay(float img_x, float img_y);
    private:
        camera_type cam_type_;
        Vec3 cam_eye_;
        Vec3 cam_up_;
        Vec3 cam_fwd_;
        Vec3 cam_right_;
        float half_angle_fov_;
        float near_plane_;  // distance from eye to the image plane
};
#endif  // CAMERA_H