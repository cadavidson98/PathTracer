#include "assimp_file_loader.h"
#include "config.h"

#include "mat/cook_torrence.h"
#include "mat/disney_principled.h"

#include "geom/camera.h"
#include "geom/triangle.h"
#include "geom/triangle_mesh.h"

#include "math/mat4.h"
#include "math/vec.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <fstream>
#include <iostream>



AssimpFileLoader::AssimpFileLoader() {
}

std::shared_ptr<cblt::Scene> AssimpFileLoader::LoadScene(std::string file_name) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file_name,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_JoinIdenticalVertices);
    cblt::Camera cam(cblt::Vec3(0.f, 0.f, -5.f));
    
    if (scene) {
        if (scene->mNumCameras > 0) {
            cam = ProcessCamera(scene, scene->mRootNode->FindNode(scene->mCameras[0]->mName), scene->mCameras[0], scene_data_);
        }
        
        for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
            ProcessMaterial(scene, scene->mMaterials[i], scene_data_);
        }

        for (unsigned int i = 0; i < scene->mNumLights; ++i) {
            aiNode *light_node = scene->mRootNode->FindNode(scene->mLights[i]->mName);
            ProcessLight(scene, light_node, scene->mLights[i], scene_data_);
        }

        ProcessNode(scene, scene->mRootNode, scene_data_);
        // build the bvh
        scene_data_->scene_triangles_ = bvh(scene_data_->tris_);
    }
    return nullptr;
}

cblt::Camera AssimpFileLoader::ProcessCamera(const aiScene* ai_scene, aiNode* cam_node, aiCamera* camera) {
    aiMatrix4x4 cam_mat, local_transform;
    camera->GetCameraMatrix(local_transform);
    aiNode* cur_node = cam_node;
    while (cur_node != NULL) {
        cam_mat = cam_mat * cur_node->mTransformation;
        cur_node = cur_node->mParent;
    }
    cam_mat = cam_mat * local_transform;
    // Rotating vectors can't be done with Assimp, since they assume all vector3 are points
    // Convert to Matrix 4x4
    float mat_data[] = { cam_mat.a1, cam_mat.b1, cam_mat.c1, cam_mat.d1,
                         cam_mat.a2, cam_mat.b2, cam_mat.c2, cam_mat.d2,
                         cam_mat.a3, cam_mat.b3, cam_mat.c3, cam_mat.d3,
                         cam_mat.a4, cam_mat.b4, cam_mat.c4, cam_mat.d4 };
    cblt::Mat4 cam_rot(mat_data);
    cblt::Vec4 origin(0, 0, 0, 1);
    cblt::Vec4 eye = cam_rot * origin;
    cblt::Vec4 forward = cam_rot * cblt::Vec4(camera->mLookAt.x, camera->mLookAt.y, camera->mLookAt.z, 0);
    cblt::Vec4 up = cam_rot * cblt::Vec4(camera->mUp.x, camera->mUp.y, camera->mUp.z, 0);
    return cblt::Camera(cblt::Vec3(eye.x, eye.y, eye.z),
        cblt::Vec3(forward.x, forward.y, forward.z),
        cblt::Vec3(up.x, up.y, up.z),
        camera->mHorizontalFOV*.5f);
}

std::shared_ptr<cblt::Geometry> AssimpFileLoader::ProcessMesh(const aiScene* scene, aiNode* mesh_node, aiMesh* mesh) {
    aiNode* cur_node = mesh_node;
    aiMatrix4x4 trans;
    // Get the specific transformation for this mesh
    while (cur_node != NULL) {
        trans = trans * cur_node->mTransformation;
        cur_node = cur_node->mParent;
    }
    // convert it to Matrix4x4 so we can perform homogeneous multiplication
    float mat_data[] = { trans.a1, trans.b1, trans.c1, trans.d1,
                         trans.a2, trans.b2, trans.c2, trans.d2,
                         trans.a3, trans.b3, trans.c3, trans.d3,
                         trans.a4, trans.b4, trans.c4, trans.d4 };
    cblt::Mat4 transform(mat_data);
    std::string mat_name(scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str());
    cblt::Material *mesh_mat = nullptr; //my_scene->mats_[mat_name];
    std::vector<cblt::Vec3> pts;
    std::vector<cblt::Vec3> norms;
    std::vector<cblt::Vec3> tans;
    std::vector<cblt::Vec3> bitans;
    std::vector<cblt::Vec2> uvs;
    std::vector<std::shared_ptr<cblt::Triangle>> mesh_tris;
    for (unsigned int k = 0; k < mesh->mNumFaces; ++k) {
        //scene_tri.mat_ = mesh_mat;
        aiFace tri = mesh->mFaces[k];

        unsigned int index = tri.mIndices[0];
        aiVector3D vert = mesh->mVertices[index];
        cblt::Vec3 p1_(vert.x, vert.y, vert.z);
        
        index = tri.mIndices[1];
        vert = mesh->mVertices[index];
        
        cblt::Vec3 p2_(vert.x, vert.y, vert.z);
        
        index = tri.mIndices[2];
        vert = mesh->mVertices[index];
        cblt::Vec3 p3_(vert.x, vert.y, vert.z);

        pts.push_back(p1_);
        pts.push_back(p2_);
        pts.push_back(p3_);

        if (mesh->HasNormals()) {
            index = tri.mIndices[0];
            aiVector3D norm = mesh->mNormals[index];
            cblt::Vec3 n1_ = cblt::Vec3(norm.x, norm.y, norm.z);

            index = tri.mIndices[1];
            norm = mesh->mNormals[index];
            cblt::Vec3 n2_ = cblt::Vec3(norm.x, norm.y, norm.z);

            index = tri.mIndices[2];
            norm = mesh->mNormals[index];
            cblt::Vec3 n3_ = cblt::Vec3(norm.x, norm.y, norm.z);

            norms.push_back(n1_);
            norms.push_back(n2_);
            norms.push_back(n3_);
        }
        if (mesh->HasTangentsAndBitangents()) {
            index = tri.mIndices[0];
            aiVector3D tan = mesh->mTangents[index];
            cblt::Vec3 t1_ = cblt::Vec3(tan.x, tan.y, tan.z);

            aiVector3D bitan = mesh->mBitangents[index];
            cblt::Vec3 bt1_ = cblt::Vec3(bitan.x, bitan.y, bitan.z);

            index = tri.mIndices[1];
            tan = mesh->mTangents[index];
            cblt::Vec3 t2_ = cblt::Vec3(tan.x, tan.y, tan.z);

            bitan = mesh->mBitangents[index];
            cblt::Vec3 bt2_ = cblt::Vec3(bitan.x, bitan.y, bitan.z);

            index = tri.mIndices[2];
            tan = mesh->mTangents[index];
            cblt::Vec3 t3_ = cblt::Vec3(tan.x, tan.y, tan.z);

            cblt::Vec3 bt3_ = cblt::Vec3(bitan.x, bitan.y, bitan.z);

            tans.push_back(t1_);
            tans.push_back(t2_);
            tans.push_back(t3_);

            tans.push_back(bt1_);
            tans.push_back(bt2_);
            tans.push_back(bt3_);
        }
        if (mesh->HasTextureCoords(0)) {
            index = tri.mIndices[0];
            aiVector3D ai_uv = mesh->mTextureCoords[0][index];
            cblt::Vec2 uv1_ = cblt::Vec2(ai_uv.x, ai_uv.y);

            index = tri.mIndices[1];
            ai_uv = mesh->mTextureCoords[0][index];
            cblt::Vec2 uv2_ = cblt::Vec2(ai_uv.x, ai_uv.y);

            index = tri.mIndices[2];
            ai_uv = mesh->mTextureCoords[0][index];
            cblt::Vec2 uv3_ = cblt::Vec2(ai_uv.x, ai_uv.y);
        }
    }
}

void AssimpFileLoader::ProcessNode(const aiScene* scene, aiNode* node, shared_ptr<cblt::Scene> &my_scene) {
    int num = node->mNumChildren;
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        ProcessMesh(scene, node, scene->mMeshes[node->mMeshes[i]], my_scene);
    }
    for (int i = 0; i < num; ++i) {
        aiNode* child = node->mChildren[i];
        ProcessNode(scene, child, my_scene);
    }
}
/*
void AssimpFileLoader::ProcessLight(const aiScene* ai_scene, aiNode* light_node, aiLight* light, shared_ptr<cblt::Scene> &my_scene) {
    aiNode* cur_node = light_node;
    aiMatrix4x4 trans;
    // Get the specific transformation for this mesh
    while (cur_node != NULL) {
        trans = trans * cur_node->mTransformation;
        cur_node = cur_node->mParent;
    }
    // convert it to Matrix4x4 so we can perform homogeneous multiplication
    float mat_data[] = { trans.a1, trans.b1, trans.c1, trans.d1,
                         trans.a2, trans.b2, trans.c2, trans.d2,
                         trans.a3, trans.b3, trans.c3, trans.d3,
                         trans.a4, trans.b4, trans.c4, trans.d4, };
    cblt::Mat4 transform(mat_data);
    Light* new_light = new Light;
    // transform the light data
    switch (light->mType) {
    case aiLightSourceType::aiLightSource_POINT: {
        new_light->type_ = LightType::POINT;
        new_light->src_ = transform * Vec4(light->mPosition.x, light->mPosition.y, light->mPosition.z, 1);
        new_light->clr_ = Color(light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b);
        new_light->dim_.x = light->mAttenuationConstant;
        new_light->dim_.y = light->mAttenuationLinear;
        new_light->dim_.z = light->mAttenuationQuadratic;
        break;
    }
    case aiLightSourceType::aiLightSource_DIRECTIONAL: {
        new_light->type_ = LightType::DIR;
        new_light->src_ = transform * Vec4(light->mDirection.x, light->mDirection.y, light->mDirection.z, 0);
        new_light->clr_ = Color(light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b);
        break;
    }
    case aiLightSourceType::aiLightSource_SPOT: {
        new_light->type_ = LightType::SPOT;
        new_light->src_ = transform * Vec4(light->mPosition.x, light->mPosition.y, light->mPosition.z, 1);
        new_light->angle1_ = light->mAngleInnerCone;
        new_light->angle2_ = light->mAngleOuterCone;
        new_light->clr_ = Color(light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b);
        break; 
    }
    default:
    break;
    }
    my_scene->lights_.push_back(new_light);
}*/

// Get the material parameters- ambient, diffuse, specular, emissive and load
// optional textures
void AssimpFileLoader::ProcessMaterial(const aiScene* scene, aiMaterial* mat, shared_ptr<cblt::Scene> &my_scene) {
    // start with ambient
    aiColor3D a, d, s, t, e;
    float ior(0), rough(-1), metalness(0);
    mat->Get(AI_MATKEY_COLOR_AMBIENT, a);
    mat->Get(AI_MATKEY_COLOR_DIFFUSE, d);
    mat->Get(AI_MATKEY_COLOR_SPECULAR, s);
    mat->Get(AI_MATKEY_COLOR_TRANSPARENT, t);
    mat->Get(AI_MATKEY_COLOR_EMISSIVE, e);
    mat->Get(AI_MATKEY_REFRACTI, ior);
    mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, rough);
    if(rough == -1) {
        mat->Get(AI_MATKEY_SHININESS, rough);
    }
    mat->Get(AI_MATKEY_REFLECTIVITY, metalness);
    int num_base = mat->GetTextureCount(aiTextureType_BASE_COLOR);
    int num_diffuse = mat->GetTextureCount(aiTextureType_DIFFUSE);
    int num_normal = mat->GetTextureCount(aiTextureType_NORMALS);
    int num_metal = mat->GetTextureCount(aiTextureType_METALNESS);
    cblt::Material *new_material;
    if (e.r + e.g + e.b > 0.f)
    {
        new_material = new cblt::CookTorrenceMaterial(Color(d.r, d.g, d.b), Color(s.r, s.g, s.b), Color(e.r, e.g, e.b), ior, rough, metalness);
    }
    else
    {
        new_material = new cblt::DisneyPrincipledMaterial(Color(d.r, d.g, d.b), 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
    }
    for (int i = 0; i < num_diffuse; ++i) {
        int idx = ProcessTexture(mat, i, aiTextureType_DIFFUSE, my_scene);
        new_material->albedo_map_ = my_scene->textures_[idx];
    }

    for (int i = 0; i < num_normal; ++i) {
        int idx = ProcessTexture(mat, i, aiTextureType_NORMALS, my_scene);
        new_material->normal_map_ = my_scene->textures_[idx];
    }
    std::string name(mat->GetName().C_Str());
    my_scene->mats_.emplace(name, new_material);
}

int AssimpFileLoader::ProcessTexture(aiMaterial* mat, int index, aiTextureType type, shared_ptr<cblt::Scene> &my_scene) {
    aiString tex_name;
    mat->GetTexture(type, index, &tex_name);
    const char* texture_path = tex_name.C_Str();
    Image * texture = new Image(texture_path);
    my_scene->textures_.push_back(texture);
    return static_cast<int>(my_scene->textures_.size()) - 1;
}