#include "file_loader.h"
#include "dielectric_material.h"
#include "camera.h"
#include "config.h"
#include "matrix.h"
#include "vec.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <fstream>
#include <iostream>

FileLoader::FileLoader() {
    scene_data_ = std::shared_ptr<Scene>(new Scene);
}

shared_ptr<Scene> FileLoader::LoadScene(string file_name) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file_name,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_JoinIdenticalVertices);
    if (scene) {
        if (scene->mNumCameras > 0) {
            ProcessCamera(scene, scene->mRootNode->FindNode(scene->mCameras[0]->mName), scene->mCameras[0], scene_data_);
        }
        else {
            scene_data_->camera_ = Camera(Vec3(0, 0, -5));
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
    return scene_data_;
}

void FileLoader::ProcessCamera(const aiScene* ai_scene, aiNode* cam_node, aiCamera* camera, shared_ptr<Scene> my_scene) {
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
    Matrix4x4 cam_rot(mat_data);
    Vec4 origin(0, 0, 0, 1);
    Vec4 eye = cam_rot * origin;
    Vec4 forward = cam_rot * Vec4(camera->mLookAt.x, camera->mLookAt.y, camera->mLookAt.z, 0);
    Vec4 up = cam_rot * Vec4(camera->mUp.x, camera->mUp.y, camera->mUp.z, 0);
    my_scene->camera_ = Camera(Vec3(eye.x, eye.y, eye.z),
        Vec3(forward.x, forward.y, forward.z),
        Vec3(up.x, up.y, up.z),
        camera->mHorizontalFOV);
}

void FileLoader::ProcessMesh(const aiScene* scene, aiNode* mesh_node, aiMesh* mesh, std::shared_ptr<Scene> my_scene) {
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
    Matrix4x4 transform(mat_data);
    std::string mat_name(scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str());
    Material *mesh_mat = my_scene->mats_[mat_name];
    std::vector<Triangle*> mesh_tris;
    for (unsigned int k = 0; k < mesh->mNumFaces; ++k) {
        Triangle* scene_tri = new Triangle;
        scene_tri->mat_ = mesh_mat;
        aiFace tri = mesh->mFaces[k];

        unsigned int index = tri.mIndices[0];
        aiVector3D vert = mesh->mVertices[index];
        vert = trans * vert;
        scene_tri->p1_ = Vec3(vert.x, vert.y, vert.z);
        
        index = tri.mIndices[1];
        vert = mesh->mVertices[index];
        vert = trans * vert;
        
        scene_tri->p2_ = Vec3(vert.x, vert.y, vert.z);
        
        index = tri.mIndices[2];
        vert = mesh->mVertices[index];
        vert = trans * vert;
        scene_tri->p3_ = Vec3(vert.x, vert.y, vert.z);

        mesh_tris.push_back(scene_tri);
    }

    if (mesh->HasNormals()) {
        for (unsigned int k = 0; k < mesh->mNumFaces; ++k) {
            aiFace tri = mesh->mFaces[k];

            unsigned int index = tri.mIndices[0];
            aiVector3D ai_norm = mesh->mNormals[index];
            Vec4 norm(ai_norm.x, ai_norm.y, ai_norm.z, 0);
            norm = transform * norm;
            mesh_tris[k]->n1_ = Vec3(norm.x, norm.y, norm.z);

            index = tri.mIndices[1];
            ai_norm = mesh->mNormals[index];
            norm = Vec4(ai_norm.x, ai_norm.y, ai_norm.z, 0);
            norm = transform * norm;
            mesh_tris[k]->n2_ = Vec3(norm.x, norm.y, norm.z);


            index = tri.mIndices[2];
            ai_norm = mesh->mNormals[index];
            norm = Vec4(ai_norm.x, ai_norm.y, ai_norm.z, 0);
            norm = transform * norm;
            mesh_tris[k]->n3_ = Vec3(norm.x, norm.y, norm.z);
        }
    }
    else {
        for (unsigned int k = 0; k < mesh->mNumFaces; ++k) {
            Vec3 norm = (mesh_tris[k]->p2_ - mesh_tris[k]->p1_).Cross(mesh_tris[k]->p3_ - mesh_tris[k]->p1_);
            norm.Normalize();

            mesh_tris[k]->n1_ =
            mesh_tris[k]->n2_ =
            mesh_tris[k]->n3_ = norm;
        }
    }
    if (mesh->HasTangentsAndBitangents()) {
        for (unsigned int k = 0; k < mesh->mNumFaces; ++k) {
            aiFace tri = mesh->mFaces[k];

            unsigned int index = tri.mIndices[0];
            aiVector3D ai_tan = mesh->mTangents[index];
            Vec4 tan(ai_tan.x, ai_tan.y, ai_tan.z, 0);
            tan = transform * tan;
            mesh_tris[k]->t1_ = Vec3(tan.x, tan.y, tan.z);

            aiVector3D ai_bitan = mesh->mBitangents[index];
            Vec4 bitan(ai_bitan.x, ai_bitan.y, ai_bitan.z, 0);
            bitan = transform * bitan;
            mesh_tris[k]->bt1_ = Vec3(bitan.x, bitan.y, bitan.z);

            index = tri.mIndices[1];
            ai_tan = mesh->mTangents[index];
            tan = Vec4(ai_tan.x, ai_tan.y, ai_tan.z, 0);
            tan = transform * tan;
            mesh_tris[k]->t2_ = Vec3(tan.x, tan.y, tan.z);

            ai_bitan = mesh->mBitangents[index];
            bitan = Vec4(ai_bitan.x, ai_bitan.y, ai_bitan.z, 0);
            bitan = transform * bitan;
            mesh_tris[k]->bt2_ = Vec3(bitan.x, bitan.y, bitan.z);


            index = tri.mIndices[2];
            ai_tan = mesh->mTangents[index];
            tan = Vec4(ai_tan.x, ai_tan.y, ai_tan.z, 0);
            tan = transform * tan;
            mesh_tris[k]->t3_ = Vec3(tan.x, tan.y, tan.z);

            ai_bitan = mesh->mBitangents[index];
            bitan = Vec4(ai_bitan.x, ai_bitan.y, ai_bitan.z, 0);
            bitan = transform * bitan;
            mesh_tris[k]->bt3_ = Vec3(bitan.x, bitan.y, bitan.z);
        }
    }
    unsigned int val = *mesh->mNumUVComponents;
    if (mesh->HasTextureCoords(0)) {
        for (unsigned int k = 0; k < mesh->mNumFaces; ++k) {
            aiFace tri = mesh->mFaces[k];

            unsigned int index = tri.mIndices[0];
            aiVector3D ai_uv = mesh->mTextureCoords[0][index];
            mesh_tris[k]->uv1_ = Vec2(ai_uv.x, ai_uv.y);

            index = tri.mIndices[1];
            ai_uv = mesh->mTextureCoords[0][index];
            mesh_tris[k]->uv2_ = Vec2(ai_uv.x, ai_uv.y);

            index = tri.mIndices[2];
            ai_uv = mesh->mTextureCoords[0][index];
            mesh_tris[k]->uv3_ = Vec2(ai_uv.x, ai_uv.y);
        }
    }
    my_scene->tris_.insert(my_scene->tris_.end(), mesh_tris.begin(), mesh_tris.end());
}

void FileLoader::ProcessNode(const aiScene* scene, aiNode* node, shared_ptr<Scene> my_scene) {
    int num = node->mNumChildren;
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        ProcessMesh(scene, node, scene->mMeshes[node->mMeshes[i]], my_scene);
    }
    for (int i = 0; i < num; ++i) {
        aiNode* child = node->mChildren[i];
        ProcessNode(scene, child, my_scene);
    }
}

void FileLoader::ProcessLight(const aiScene* ai_scene, aiNode* light_node, aiLight* light, shared_ptr<Scene> my_scene) {
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
    Matrix4x4 transform(mat_data);
    Light* new_light = new Light;
    // transform the light data
    switch (light->mType) {
    case aiLightSourceType::aiLightSource_POINT: {
        Vec4 light_pos = transform * Vec4(light->mPosition.x, light->mPosition.y, light->mPosition.z, 1);

        new_light->type_ = LightType::point;
        new_light->pos_ = Vec3(light_pos.x, light_pos.y, light_pos.z);
        new_light->clr_ = Color(light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b);
        new_light->attenu_const_ = light->mAttenuationConstant;
        new_light->attenu_lin_ = light->mAttenuationLinear;
        new_light->attenu_quad_ = light->mAttenuationQuadratic;
        break;
    }
    case aiLightSourceType::aiLightSource_DIRECTIONAL: {
        Vec4 light_dir = transform * Vec4(light->mDirection.x, light->mDirection.y, light->mDirection.z, 0);

        new_light->type_ = LightType::dir;
        new_light->dir1_ = Vec3(light_dir.x, light_dir.y, light_dir.z);
        new_light->clr_ = Color(light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b);
        break;
    }
    case aiLightSourceType::aiLightSource_SPOT: {
        Vec4 light_pos = transform * Vec4(light->mPosition.x, light->mPosition.y, light->mPosition.z, 1);

        new_light->type_ = LightType::spot;
        new_light->pos_ = Vec3(light_pos.x, light_pos.y, light_pos.z);
        new_light->angle1_ = light->mAngleInnerCone;
        new_light->angle2_ = light->mAngleOuterCone;
        new_light->clr_ = Color(light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b);
        break; 
    }
    default:
    break;
    }
    my_scene->lights_.push_back(new_light);
}

// Get the material parameters- ambient, diffuse, specular, emissive and load
// optional textures
void FileLoader::ProcessMaterial(const aiScene* scene, aiMaterial* mat, shared_ptr<Scene> my_scene) {
    // start with ambient
    aiColor3D a, d, s, t, e;
    float ior(0), rough(0);
    mat->Get(AI_MATKEY_COLOR_AMBIENT, a);
    mat->Get(AI_MATKEY_COLOR_DIFFUSE, d);
    mat->Get(AI_MATKEY_COLOR_SPECULAR, s);
    mat->Get(AI_MATKEY_COLOR_TRANSPARENT, t);
    mat->Get(AI_MATKEY_COLOR_EMISSIVE, e);
    mat->Get(AI_MATKEY_REFRACTI, ior);
    mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, rough);
    int num_base = mat->GetTextureCount(aiTextureType_BASE_COLOR);
    int num_diffuse = mat->GetTextureCount(aiTextureType_DIFFUSE);
    int num_normal = mat->GetTextureCount(aiTextureType_NORMALS);
    int num_metal = mat->GetTextureCount(aiTextureType_METALNESS);
    Material *new_material = new DielectricMaterial(Color(d.r, d.g, d.b), Color(s.r, s.g, s.b), Color(e.r, e.g, e.b), ior, rough);

    /*for (int i = 0; i < num_diffuse; ++i) {
        int idx = ProcessTexture(mat, i, aiTextureType_DIFFUSE, my_scene);
        new_material->diffuse_map_ = my_scene->textures_[idx];
    }

    for (int i = 0; i < num_normal; ++i) {
        int idx = ProcessTexture(mat, i, aiTextureType_NORMALS, my_scene);
        new_material->normal_map_ = my_scene->textures_[idx];
    }*/
    std::string name(mat->GetName().C_Str());
    my_scene->mats_.emplace(name, new_material);
}

int FileLoader::ProcessTexture(aiMaterial* mat, int index, aiTextureType type, shared_ptr<Scene> my_scene) {
    aiString tex_name;
    mat->GetTexture(type, index, &tex_name);
    const char* texture_path = tex_name.C_Str();
    Image * texture = new Image(texture_path);
    my_scene->textures_.push_back(texture);
    return static_cast<int>(my_scene->textures_.size()) - 1;
}