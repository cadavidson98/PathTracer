#include "legacy_file_loader.h"
#include "cook_torrence.h"
#include "disney_principled.h"

#include "vec.h"
#include "math_helpers.h"

#include <fstream>
#include <memory>

std::shared_ptr<Scene> LegacyFileLoader::LoadScene(std::string file_name) {
    std::ifstream in_file(file_name);
    std::shared_ptr<Scene> new_scene(new Scene);
    if (!in_file.good()) {
        return new_scene;
    }
    // text-based OBJ style, can process line-by-line
    // The default material is a matte white
    int num_mats = 1;
    Material *cur_mat = new CookTorrenceMaterial(
            Color(1.f, 1.f, 1.f), 
            Color(1.f, 1.f, 1.f), 
            Color(0.f, 0.f, 0.f),
            1.f, 1.f, 0.0f);
    new_scene->mats_["matte white"] = cur_mat;

    std::vector<float> verts;
    std::vector<float> norms;
    // The maximum and minimum bounds of the scene. Used when generating the BVH
    Vec3 min_pt(INFINITY, INFINITY, INFINITY), max_pt(-INFINITY, -INFINITY, -INFINITY);
    int max_vert(-1), max_norm(-1), nindex(0), vindex(0), tindex(0);
    // The Camera variables
    Vec3 cam_eye, cam_fwd, cam_up;
    float cam_FOV, cam_dist;
    // Read each line in the file
    std::string command, line;
    while (!in_file.eof()) {
        in_file >> command;
        if (!command.compare("camera_fwd:")) {
            in_file >> cam_fwd.x >> cam_fwd.y >> cam_fwd.z;
        }
        else if (!command.compare("camera_up:")) {
            in_file >> cam_up.x >> cam_up.y >> cam_up.z;
        }
        else if (!command.compare("camera_pos:")) {
            in_file >> cam_eye.x >> cam_eye.y >> cam_eye.z;
        }
        else if (!command.compare("camera_fov_ha:")) {
            in_file >> cam_FOV;
            cam_FOV = RMth::toRadians(cam_FOV); // convert to radians
        }
        else if (!command.compare("camera_near:")) {
            in_file >> cam_dist;
        }
        else if (!command.compare("material:")) {
            Color albedo, specular, emissive;
            float ior(1.f), rough(0.f), metal(0.f);
            in_file >> albedo.r >> albedo.g >> albedo.b
                    >> specular.r >> specular.g >> specular.b
                    >> emissive.r >> emissive.g >> emissive.b
                    >> ior >> rough >> metal;
            if (emissive.r + emissive.g + emissive.b > 0.f || metal == 0.f)
            {
                cur_mat = new CookTorrenceMaterial(albedo, specular, emissive, ior, rough, metal);
            }
            else
            {
                cur_mat = new DisneyPrincipledMaterial(Color(albedo.r, albedo.g, albedo.b), 0.f, metal, 0.f, 0.f, rough, 1.f, 0.f, 0.f, 0.f, 0.f);
            }
            
            new_scene->mats_["Material " + std::to_string(num_mats++)] = cur_mat;
        }
        else if (!command.compare("max_vertices:")) {
            in_file >> max_vert;
            // Reserve space for the vertices
            verts.reserve(3 * max_vert);
        }
        else if (!command.compare("max_normals:")) {
            in_file >> max_norm;
            // Reserve space for the normals
            norms.reserve(3 * max_norm);
            norms.resize(3 * max_norm);
        }
        else if (!command.compare("vertex:")) {
            // Add another vertex to the master list
            float vert_x, vert_y, vert_z;
            in_file >> vert_x >> vert_y >> vert_z;
            verts.push_back(vert_x);
            verts.push_back(vert_y);
            verts.push_back(vert_z);
        }
        else if (!command.compare("normal:")) {
            // Add another normal to the master list
            in_file >> norms[nindex++] >> norms[nindex++] >> norms[nindex++];
        }
        else if (!command.compare("triangle:")) {
            int p1, p2, p3;
            Triangle new_tri;
            in_file >> p1 >> p2 >> p3;
            if(p1 >= verts.size() || p2 >= verts.size() || p3 >= verts.size()) {
                continue;
            }
            // Find the corresponding vertices in the vertex array and add them to the triangle
            new_tri.p1_ = Vec3(verts[p1*3], verts[p1*3 + 1], verts[p1*3 + 2]);
            new_tri.p2_ = Vec3(verts[p2*3], verts[p2*3 + 1], verts[p2*3 + 2]);
            new_tri.p3_ = Vec3(verts[p3*3], verts[p3*3 + 1], verts[p3*3 + 2]);
            // now get the face normal
            Vec3 face = (new_tri.p2_ - new_tri.p1_).Cross(new_tri.p3_ - new_tri.p1_);
            face.Normalize();
            new_tri.n1_ = 
            new_tri.n2_ = 
            new_tri.n3_ = face;
            new_tri.mat_ = cur_mat;
            new_scene->tris_.push_back(new_tri);
        }
        else if (!command.compare("normal_triangle:")) {
            int p1, p2, p3, n1, n2, n3;
            in_file >> p1 >> p2 >> p3 >> n1 >> n2 >> n3;
            if(p1 >= verts.size() || p2 >= verts.size() || p3 >= verts.size()
            || n1 >= norms.size() || n2 >= norms.size() || n3 >= norms.size()) {
                continue;
            }
            Triangle new_tri;
            new_tri.p1_ = Vec3(verts[p1*3], verts[p1*3 + 1], verts[p1*3 + 2]);
            new_tri.p2_ = Vec3(verts[p2*3], verts[p2*3 + 1], verts[p2*3 + 2]);
            new_tri.p3_ = Vec3(verts[p3*3], verts[p3*3 + 1], verts[p3*3 + 2]);
            
            new_tri.n1_ = Vec3(norms[n1*3], norms[n1*3 + 1], norms[n1*3 + 2]);
            new_tri.n2_ = Vec3(norms[n2*3], norms[n2*3 + 1], norms[n2*3 + 2]);
            new_tri.n3_ = Vec3(norms[n3*3], norms[n3*3 + 1], norms[n3*3 + 2]);
            
            new_tri.mat_ = cur_mat;
            new_scene->tris_.push_back(new_tri);
        }
        /*else if (!command.compare("background:")) {
            // Deprecated
            in_file >> b_clr[0] >> b_clr[1] >> b_clr[2];
        }*/
        else if (!command.compare("point_light:")) {
            Vec4 pos(0.f, 0.f, 0.f, 1.f);
            Vec3 falloff(0.f, 0.f, 1.f);
            Color clr;
            in_file >> clr.r >> clr.g >> clr.b
                    >> pos.x >> pos.y >> pos.z;
            Light *pt_light = new Light{LightType::POINT, clr, pos, falloff, Vec3(), Vec3(), 0.f, 0.f};
            new_scene->lights_.push_back(pt_light);
        }
        else if (!command.compare("directional_light:")) {
            Vec4 pos(0.f);
            Color clr;
            Vec3 falloff;
            in_file >> clr.r >> clr.g >> clr.b
                    >> pos.x >> pos.y >> pos.z;
            Light *dir_light = new Light{LightType::DIR, clr, pos, falloff, Vec3(), Vec3(), 0.f, 0.f};
            new_scene->lights_.push_back(dir_light);
        }
        else if(!command.compare("quad_light:")) {
            Vec4 pos(0.f, 0.f, 0.f, 1.f);
            Color clr;
            Vec3 dims, left, up;
            in_file >> clr.r >> clr.g >> clr.b
                    >> pos.x >> pos.y >> pos.z
                    >> dims.x >> dims.y
                    >> left.x >> left.y >> left.z
                    >> up.x >> up.y >> up.z;
            Light *quad_light = new Light{LightType::AREA, clr, pos, dims, left, up, 0.f, 0.f};
            new_scene->lights_.push_back(quad_light);
        }
        else {
            // Unsupported command or comment, just skip it
            std::getline(in_file, line);
            continue;
        }
    }
    // build the BVH for faster intersection tests
    new_scene->camera_ = Camera(cam_eye, cam_fwd, cam_up, cam_FOV);
    new_scene->scene_triangles_ = bvh(new_scene->tris_);
    return new_scene;
}