#ifndef SCENE_H
#define SCENE_H

#include "bvh.h"
#include "geom/camera.h"
#include "structs.h"
#include "mat/image_lib.h"
#include "mat/Material.h"

#include <memory>
#include <vector>
#include <unordered_map>

/**
 * @brief A simple structure for describing a 3D scene file. Includes everything necessary to render a scene
 * 
 */
class Scene {
	public:
		Scene();
		~Scene();
		std::vector<Light*> lights_;  //! The lights in the scene
		cblt::Camera camera_;  //! The camera from when we render the scene
		bvh scene_triangles_;  //! The triangles which describe the scene geometry
		std::vector<Triangle> tris_;  //! Temporary location for all the triangles in the scene
		std::unordered_map<std::string, cblt::Material*> mats_;  //! The materials which describe how light is reflected at each triangle
		std::vector<Image*> textures_;  //! the textures which cover some triangles
};

#endif // SCENE_H
