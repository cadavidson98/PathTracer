#ifndef SDESC_FILE_LOADER_H
#define SDESC_FILE_LOADER_H

#include "file_loader.h"

#include "mat/material.h"

#include <pugixml.hpp>

#include <unordered_map>
#include <memory>
#include <string>

class SDescFileLoader final : public FileLoader {
    public:
    std::shared_ptr<cblt::Scene> LoadScene(std::string file_name) override;
    private:
    bool ProcessCamera(pugi::xml_node &camera_node);
    bool ProcessPrincipledMaterial(pugi::xml_node &mat_node);
    bool ProcessTorrenceMaterial(pugi::xml_node &mat_node);
    bool ProcessMesh(pugi::xml_node &mesh_node);
    bool ProcessPrim(pugi::xml_node &elem_node);

    cblt::Camera cam_;
    std::unordered_map<std::string, std::shared_ptr<cblt::Material>> material_map_;
    std::unordered_map<std::string, std::shared_ptr<cblt::Geometry>> mesh_map_;
};
#endif  // SDESC_FILE_LOADER_H