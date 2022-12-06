#ifndef ASSIMP_FILE_LOADER_H
#define ASSIMP_FILE_LOADER_H

#include "file_loader.h"
#include "geom/scene.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <memory>
#include <string>

class AssimpFileLoader final : public FileLoader {
    public:
        AssimpFileLoader();
        std::shared_ptr<cblt::Scene> LoadScene(std::string file_name) override;
    private:
        cblt::Camera ProcessCamera(const aiScene* ai_scene, aiNode* cam_node, aiCamera* cam);
        std::shared_ptr<cblt::Geometry> ProcessMesh(const aiScene* ai_scene, aiNode *mesh_node, aiMesh* mesh);
        void ProcessNode(const aiScene* ai_scene, aiNode *node, std::shared_ptr<cblt::Scene> &my_scene);
        //void ProcessLight(const aiScene* ai_scene, aiNode* light_node, aiLight* light, std::shared_ptr<cblt::Scene> &my_scene);
        void ProcessMaterial(const aiScene* ai_scene, aiMaterial* mat, std::shared_ptr<cblt::Scene> &my_scene);
        int ProcessTexture(aiMaterial* mat, int index, aiTextureType type, std::shared_ptr<cblt::Scene> &my_scene);
};

#endif  // ASSIMP_FILE_LOADER_H