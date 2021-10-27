#include "scene.h"

using namespace std;

Scene::Scene() {

}

Scene::~Scene() {
	for (int i = 0; i < tris_.size(); ++i) {
		delete tris_[i];
	}
	for (unordered_map<std::string, Material*>::iterator iter = mats_.begin(); iter != mats_.end(); iter++) {
		delete iter->second;
	}
	for (int i = 0; i < lights_.size(); ++i) {
		delete lights_[i];
	}
	for (int i = 0; i < textures_.size(); ++i) {
		delete textures_[i];
	}
}