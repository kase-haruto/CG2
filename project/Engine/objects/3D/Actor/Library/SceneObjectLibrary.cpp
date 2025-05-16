#include "SceneObjectLibrary.h"

#include <Engine/Renderer/Mesh/MeshRenderer.h>
#include <algorithm>

SceneObjectLibrary::SceneObjectLibrary(){
	allSceneObjects_.clear();
}

void SceneObjectLibrary::AddObject(SceneObject* object){
	if (std::find(allSceneObjects_.begin(), allSceneObjects_.end(), object) == allSceneObjects_.end()){
		allSceneObjects_.push_back(object);
	}
}

void SceneObjectLibrary::RemoveObject(SceneObject* object){
	auto it = std::remove(allSceneObjects_.begin(), allSceneObjects_.end(), object);
	allSceneObjects_.erase(it, allSceneObjects_.end());
}

void SceneObjectLibrary::Clear(){
	allSceneObjects_.clear();
}

const std::vector<SceneObject*>& SceneObjectLibrary::GetAllObjects() const{
	return allSceneObjects_;
}

void SceneObjectLibrary::RegisterToRenderer(MeshRenderer* renderer){
	for (auto* obj : allSceneObjects_){
		obj->RegisterToRenderer(renderer);
	}
}
