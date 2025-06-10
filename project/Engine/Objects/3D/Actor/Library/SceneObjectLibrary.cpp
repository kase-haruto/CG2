#include "SceneObjectLibrary.h"

#include <Engine/Renderer/Mesh/MeshRenderer.h>
#include <algorithm>

SceneObjectLibrary::SceneObjectLibrary(){
	allSceneObjects_.clear();
}

void SceneObjectLibrary::AddObject(std::unique_ptr<SceneObject> object){
	if (!object) return;

	std::shared_ptr<SceneObject> shared = std::move(object); // unique → shared
	AddObject(shared);
}

void SceneObjectLibrary::AddObject(const std::shared_ptr<SceneObject>& object){
	if (std::find(allSceneObjects_.begin(), allSceneObjects_.end(), object) == allSceneObjects_.end()){
		allSceneObjects_.push_back(object);
	}
}

void SceneObjectLibrary::RemoveObject(const std::shared_ptr<SceneObject>& object){
	auto it = std::remove(allSceneObjects_.begin(), allSceneObjects_.end(), object);
	allSceneObjects_.erase(it, allSceneObjects_.end());
}

void SceneObjectLibrary::Clear(){
	allSceneObjects_.clear();
}

const std::vector<std::shared_ptr<SceneObject>>& SceneObjectLibrary::GetAllObjects() const{
	return allSceneObjects_;
}

void SceneObjectLibrary::RegisterToRenderer(MeshRenderer* renderer){
	for (const auto& obj : allSceneObjects_){
		if (obj){
			obj->RegisterToRenderer(renderer);
		}
	}
}