#ifndef SCENE_H
#define SCENE_H

#include "bvh.h"
#include "camera.h"
#include "structs.h"
#include "image_lib.h"
#include "Material.h"

#include <memory>
#include <vector>
#include <unordered_map>

class Scene {
	public:
		Scene();
		~Scene();
		std::vector<Light*> lights_;
		Camera camera_;
		bvh scene_triangles_;
		std::vector<Triangle*> tris_;
		std::unordered_map<std::string, Material*> mats_;
		std::vector<Image*> textures_;
};

#endif // SCENE_H
