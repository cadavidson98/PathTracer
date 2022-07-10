#include "camera.h"

#include <cmath>

Camera::Camera(Vec3 eye, Vec3 forward, Vec3 up, float half_angle_fov, camera_type proj_type) {
    cam_eye_ = eye;

    cam_fwd_ = forward;
    cam_right_ = forward.Cross(up);
    cam_up_ = cam_right_.Cross(forward);
    half_angle_fov_ = half_angle_fov;

    cam_fwd_.Normalize();
    cam_right_.Normalize();
    cam_up_.Normalize();

    cam_type_ = proj_type;
    near_plane_ = .1f;
}

void Camera::ConfigureExtent(float img_w, float img_h) {
    // calculate the distance from the eye to the image plane
    near_plane_ = (img_h * .5f) / std::tanf(half_angle_fov_);
}

Ray Camera::CreateRay(float img_x, float img_y) {
    Vec3 world_pt = cam_eye_ - near_plane_ * cam_fwd_ + img_x * cam_right_ + img_y * cam_up_;
    Ray scene_ray;
    // now make a ray based on the type
    switch(cam_type_) {
        case orthographic:
        scene_ray.pos = world_pt + near_plane_ * cam_fwd_;
        scene_ray.dir = -1 * cam_fwd_;
        scene_ray.inverse = 1.f / scene_ray.dir;
        break;
        case cavalier:
        //break;
        case cabinet:
        //break;
        case perspective:
        default:
        scene_ray.pos = cam_eye_;
        scene_ray.dir = world_pt - cam_eye_;
        scene_ray.inverse = 1.f / scene_ray.dir;
        break;
    }
    scene_ray.dir.Normalize();
    scene_ray.inverse.Normalize();
    return scene_ray;
}