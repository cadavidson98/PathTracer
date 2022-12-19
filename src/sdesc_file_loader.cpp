#include "sdesc_file_loader.h"

#include "mat/cook_torrence.h"
#include "mat/disney_principled.h"

#include "math/vec.h"
#include "math/mat4.h"
#include "math/math_helpers.h"

#include "geom/triangle_mesh.h"

#include <iostream>
#include <sstream>
#include <memory>

template<class T>
void parseString(std::istream_iterator<T> iter, std::vector<T> &vals)
{
    std::copy(iter, std::istream_iterator<T>(), std::back_inserter(vals));
}

std::shared_ptr<cblt::Scene> SDescFileLoader::LoadScene(std::string file_name)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(file_name.c_str());
    if (!result)
    {
        return nullptr;
    }
    pugi::xpath_node_set cameras = doc.select_nodes("/SDesc/Library_Cameras/camera");
    for (pugi::xpath_node node: cameras)
    {
        pugi::xml_node camera = node.node();
        ProcessCamera(camera);
    }

    pugi::xpath_node_set materials = doc.select_nodes("/SDesc/Library_Materials/Material");
    for (pugi::xpath_node node: materials)
    {
        pugi::xml_node material = node.node();
        std::string mat_type(material.select_node("material_type").node().text().as_string());
        if (!mat_type.compare("Principled Material"))
        {
            ProcessPrincipledMaterial(material);
        }
        else if (!mat_type.compare("Cook Torrence Material"))
        {
            ProcessTorrenceMaterial(material);
        }
    }

    pugi::xpath_node_set geometry = doc.select_nodes("/SDesc/Library_Geometry/geometry");
    for (pugi::xpath_node node: geometry)
    {
        pugi::xml_node geom = node.node();
        ProcessMesh(geom);
    }

    // construct the scene
    std::vector<std::shared_ptr<cblt::ScenePrim>> prims;
    pugi::xpath_node_set scene = doc.select_nodes("/SDesc/Scene/Element");
    for (pugi::xpath_node node: scene)
    {
        pugi::xml_node elem_node = node.node();
        // fetch mesh
        std::string mesh_id = elem_node.attribute("primitive_id").as_string();
        
        std::vector<float> transform_arr;
    
        std::stringstream parse_mat(elem_node.child("Transform").text().as_string());
        std::istream_iterator<float> mat_iter(parse_mat);
        parseString(mat_iter, transform_arr);
        cblt::Mat4 transform(reinterpret_cast<float*>(transform_arr.data()));
        prims.push_back(std::make_shared<cblt::ScenePrim>(mesh_map_[mesh_id], transform));
    }
    return std::make_shared<cblt::Scene>(cam_, prims);
}

bool SDescFileLoader::ProcessCamera(pugi::xml_node &camera_node)
{
    pugi::xml_node pos = camera_node.select_node("position").node();
    pugi::xml_node fwd = camera_node.select_node("forward").node();
    pugi::xml_node up = camera_node.select_node("up").node();
    pugi::xml_node fov = camera_node.select_node("FOV_ha").node();
    
    std::array<pugi::xml_node, 3> cam_nodes = { pos, fwd, up };
    std::array<std::vector<float>, 3> cam_vecs;
    std::array<cblt::Vec3, 3> cam_vals;
    
    for(int i = 0; i < cam_nodes.size(); ++i)
    {
        const pugi::char_t * text = cam_nodes[i].first_child().value();
        const char * txt = cam_nodes[i].first_child().text().as_string();
        std::stringstream parse(text);
        std::istream_iterator<float> iter(parse);
        parseString(iter, cam_vecs[i]);
    
        if (cam_vecs[i].size() < 3)
        {
            return false;
        }

        cam_vals[i].x = cam_vecs[i][0];
        cam_vals[i].y = cam_vecs[i][1];
        cam_vals[i].z = cam_vecs[i][2];
    }
    
    // fov
    float fov_val = cblt::toRadians(std::stof(fov.first_child().value()));
    cam_ = cblt::Camera(cam_vals[0], cam_vals[1], cam_vals[2], fov_val);
    return true;
}

bool SDescFileLoader::ProcessPrincipledMaterial(pugi::xml_node &mat_node)
{
    pugi::xml_node clr = mat_node.select_node("Base_Color").node();
    const char * text = clr.first_child().text().as_string();
    std::stringstream parse(text);
    std::istream_iterator<float> iter(parse);
    std::vector<float> clr_vals;
    parseString(iter, clr_vals);
    Color mat_base(clr_vals[0], clr_vals[1], clr_vals[2]);
    // get scalar attribs
    float mat_sub = mat_node.select_node("Subsurface").node().text().as_float();
    float mat_met = mat_node.select_node("Metallic").node().text().as_float();
    float mat_spec = mat_node.select_node("Specular").node().text().as_float();
    float mat_spec_tint = mat_node.select_node("Specular_Tint").node().text().as_float();
    float mat_rough = mat_node.select_node("Roughness").node().text().as_float();
    float mat_aniso = mat_node.select_node("Anisotropic").node().text().as_float();
    float mat_shn = mat_node.select_node("Sheen").node().text().as_float();
    float mat_shn_tint = mat_node.select_node("Sheen_Tint").node().text().as_float();
    float mat_clear = mat_node.select_node("Clearcoat").node().text().as_float();
    float mat_clear_coat = mat_node.select_node("Clearcoat_Gloss").node().text().as_float();
    std::shared_ptr<cblt::Material> material = std::make_shared<cblt::DisneyPrincipledMaterial>(mat_base, mat_sub, mat_met, mat_spec, mat_spec_tint, mat_rough, mat_aniso, mat_shn, mat_shn_tint, mat_clear, mat_clear_coat);
    material_map_[std::string(mat_node.attribute("ID").as_string())] = material;
    return true;
}

bool SDescFileLoader::ProcessTorrenceMaterial(pugi::xml_node &mat_node)
{
    pugi::xml_node albedo = mat_node.select_node("Albedo").node();
    pugi::xml_node specular = mat_node.select_node("Specular").node();
    pugi::xml_node emissive = mat_node.select_node("Emissive").node();
    std::array<pugi::xml_node, 3> cam_nodes = { albedo, specular, emissive };
    std::array<std::vector<float>, 3> clr_vecs;
    std::array<Color, 3> clr_vals;
    
    for(int i = 0; i < cam_nodes.size(); ++i)
    {
        const char *txt = cam_nodes[i].text().as_string();
        std::stringstream parse(txt);
        std::istream_iterator<float> iter(parse);
        parseString(iter, clr_vecs[i]);
    
        if (clr_vecs[i].size() < 3)
        {
            return false;
        }

        clr_vals[i].r = clr_vecs[i][0];
        clr_vals[i].g = clr_vecs[i][1];
        clr_vals[i].b = clr_vecs[i][2];
    }
    
    // get scalar attribs
    float mat_ior = mat_node.select_node("IOR").node().text().as_float();
    float mat_metal = mat_node.select_node("Metallic").node().text().as_float();
    float mat_rough = mat_node.select_node("Roughness").node().text().as_float();
    
    std::shared_ptr<cblt::Material> material = std::make_shared<cblt::CookTorrenceMaterial>(clr_vals[0], clr_vals[1], clr_vals[2], mat_ior, mat_rough, mat_metal);
    material_map_[std::string(mat_node.attribute("ID").as_string())] = material;
    return true;
}

bool SDescFileLoader::ProcessMesh(pugi::xml_node &mesh_node)
{
    pugi::xml_node node_verts = mesh_node.select_node("positions").node();
    pugi::xml_node node_tris = mesh_node.select_node("indices").node();
    std::string mat_name = mesh_node.select_node("surface_material").node().text().as_string();
    
    std::vector<float> verts_arr;
    std::vector<int> ind_arr;
    
    std::stringstream parse_vert(node_verts.text().as_string());
    std::istream_iterator<float> vert_iter(parse_vert);
    parseString(vert_iter, verts_arr);
    std::stringstream parse_ind(node_tris.text().as_string());
    std::istream_iterator<int> ind_iter(parse_ind);
    parseString(ind_iter, ind_arr);

    std::shared_ptr<cblt::Material> material = material_map_[mat_name];

    std::vector<std::shared_ptr<cblt::Triangle>> mesh;
    for (int i = 0; i < ind_arr.size(); i += 3)
    {
        int id1 = ind_arr[i];
        int id2 = ind_arr[i + 1];
        int id3 = ind_arr[i + 2];
        cblt::Vec3 pos1(verts_arr[3 * id1], verts_arr[3 * id1 + 1], verts_arr[3 * id1 + 2]);
        cblt::Vec3 pos2(verts_arr[3 * id2], verts_arr[3 * id2 + 1], verts_arr[3 * id2 + 2]);
        cblt::Vec3 pos3(verts_arr[3 * id3], verts_arr[3 * id3 + 1], verts_arr[3 * id3 + 2]);

        mesh.push_back(std::make_shared<cblt::Triangle>(pos1, pos2, pos3, material));
    }

    std::shared_ptr<cblt::Geometry> model = std::make_shared<cblt::TriangleMesh>(mesh);
    mesh_map_[std::string(mesh_node.attribute("ID").as_string())] = model;
    return true;
}

bool SDescFileLoader::ProcessPrim(pugi::xml_node &elem_node)
{
    return true;
}