#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include "scene.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <memory>
#include <string>

using namespace std;

class FileLoader {
    public:
        FileLoader();
        shared_ptr<Scene> LoadScene(string file_name);
    private:
        shared_ptr<Scene> scene_data_;

        void ProcessCamera(const aiScene* ai_scene, aiNode* cam_node, aiCamera* cam, shared_ptr<Scene> my_scene);
        void ProcessMesh(const aiScene* ai_scene, aiNode *mesh_node, aiMesh* mesh, shared_ptr<Scene> my_scene);
        void ProcessNode(const aiScene* ai_scene, aiNode *node, shared_ptr<Scene> my_scene);
        void ProcessLight(const aiScene* ai_scene, aiNode* light_node, aiLight* light, shared_ptr<Scene> my_scene);
        void ProcessMaterial(const aiScene* ai_scene, aiMaterial* mat, shared_ptr<Scene> my_scene);
        int ProcessTexture(aiMaterial* mat, int index, aiTextureType type, shared_ptr<Scene> my_scene);
};

#endif  // FILE_LOADER_H