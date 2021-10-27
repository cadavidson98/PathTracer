#include <fstream>
#include <iostream>
#include <chrono>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "config.h"
#include "camera.h"
#include "image_lib.h"
#include "structs.h"
#include "collision.h"
#include "file_loader.h"
#include "ray_tracer.h"

void ProcessMesh(aiMesh* mesh, aiNode* mesh_node, const aiScene* scene, std::vector<Triangle> &tris, std::vector<Material*> &mats);
void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Triangle> &tris, std::vector<Material*> &mats);
Color LightPoint(HitInfo hit, Light pos, Camera c);

int main(int argc, char* argv[]) {
    srand(time(NULL));
    std::cout << "Test" << std::endl;
    std::string file_name = std::string(DEBUG_DIR) + '/'; 
    std::string out_name("image.png");
    if(argc < 2) {
        file_name += "Elma_side.dae";
    }
    else if (argc < 3) {
        file_name += argv[1];
    }
    else {
        file_name += argv[1];
        out_name = argv[2];
    }
    std::cout << "Opening " << file_name << std::endl;
    FileLoader file_loader;
    std::shared_ptr<Scene> my_scene = file_loader.LoadScene(file_name);
    int width(400), height(300);
    int half_width = width >> 1;
    int half_height = height >> 1;
    my_scene->camera_.SetDist(half_width / std::tanf(.5f * my_scene->camera_.GetHalfFOV()));
    RayTracer ray_tracer(my_scene);
    auto s_time = std::chrono::high_resolution_clock::now();
    Image * img = ray_tracer.RayTrace(width, height);
    auto e_time = std::chrono::high_resolution_clock::now();
    auto min = std::chrono::duration_cast<std::chrono::minutes>(e_time - s_time);
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(e_time - s_time);
    auto mil = std::chrono::duration_cast<std::chrono::milliseconds>(e_time - s_time);
    std::cout << "\nRender Time\n" 
              << "\tMinutes: " << min.count() << "\n"
              << "\tSeconds: " << sec.count() - 60 * min.count() << "\n"
              << "\tMilliseconds: " << mil.count() - (60000 * min.count()) - (1000 * sec.count())  <<"\n";
    std::cout << "Change output file name (y/n)?" << std::endl;
    char new_file_name;
    std::cin >> new_file_name;
    if (new_file_name == 'y' || new_file_name == 'Y') {
        std::cout << "Enter new file name: ";
        std::cin >> out_name;
    }
    img->write(out_name.c_str());
    std::cout << "Wrote result to " << out_name << std::endl;
    return 0;
}

void ProcessMesh(aiMesh* mesh, aiNode* mesh_node, const aiScene* scene, std::vector<Triangle>& tris, std::vector<Material*> &mats) {
    
}

void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Triangle>& tris, std::vector<Material*> &mats) {
    int num = node->mNumChildren;
    for (int i = 0; i < node->mNumMeshes; ++i) {
        ProcessMesh(scene->mMeshes[node->mMeshes[i]], node, scene, tris, mats);
    }
    for (int i = 0; i < num; ++i) {
        aiNode* child = node->mChildren[i];
        ProcessNode(child, scene, tris, mats);
    }
}

Color LightPoint(HitInfo h, Light l, Camera c)
{
    Color clr(0, 0, 0);
    // get ambient light
    float time;
    float i = 1.0;
    Vec3 eye = c.Eye();
    Vec3 to_light;
    Vec3 to_eye;
    // get light vectors
    switch (l.type_) {
    case point: {
        to_light = l.pos_ - h.pos;
        to_eye = eye - h.pos;
        float dist_sqr = to_light.Dot(to_light);
        float attenuation = l.attenu_const_ + sqrt(dist_sqr) + dist_sqr;
        i = 1.0 / (attenuation + .0001);
        time = to_light.MagnitudeSqr();
        break;
    }
    case dir: {
        to_light = (l.dir1_ * -1.0).UnitVec();
        to_eye = eye - h.pos;
        time = INFINITY;
        break;
    }
    case spot: {
        // get the angle from the light and point
        to_light = l.pos_ - h.pos;
        time = to_light.MagnitudeSqr();
        float angle = std::acosf((-1 * to_light.UnitVec()).Dot((l.dir1_).UnitVec()));
        if (angle < l.angle1_) {
            // behave like point light
            to_eye = eye - h.pos;
            i = 1.0 / (to_light.Dot(to_light) + .0001);
        }
        else if (angle > l.angle2_) {
            to_eye = eye - h.pos;
            i = 0.0;
        }
        else {
            // linear falloff
            to_eye = eye - h.pos;
            i = ((l.angle2_ - angle) / (l.angle2_ - l.angle1_)) * (1.0 / (to_light.Dot(to_light) + .0001));
        }
    }
    }

    float dir = h.norm.Dot(to_light);
    Vec3 norm = h.norm;
    Vec3 r = (2 * to_light.Dot(norm) * norm - to_light).UnitVec();
    float kd = i * std::max((float)0.0, norm.Dot(to_light.UnitVec()));
    float ks = std::max((float)0.0, std::pow(r.Dot(to_eye.UnitVec()), h.m.ns_));
    Color ambient = h.m.a_;
    Color diffuse = l.clr_ * (h.m.d_ * kd);
    Color specular = l.s_ * (h.m.s_ * ks);
    clr = clr + ambient + diffuse + specular;
    if (h.m.diffuse_map_) {
        clr = clr * h.m.diffuse_map_->sample(h.uv.x, h.uv.y);
    }

    return clr;
}