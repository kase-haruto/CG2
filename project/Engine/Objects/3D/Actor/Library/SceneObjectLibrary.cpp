#include "SceneObjectLibrary.h"

SceneObjectLibrary::SceneObjectLibrary(){
	allSceneObjects_.clear();
}

void SceneObjectLibrary::AddObject(std::unique_ptr<SceneObject> object){
	allSceneObjects_.push_back(std::move(object));
}

void SceneObjectLibrary::RemoveObject(SceneObject* object){
	auto it = std::remove_if(allSceneObjects_.begin(), allSceneObjects_.end(),
							 [object] (const std::unique_ptr<SceneObject>& obj){
								 return obj.get() == object;
							 });
	allSceneObjects_.erase(it, allSceneObjects_.end());
}

void SceneObjectLibrary::Clear(){
	allSceneObjects_.clear();
}

std::vector<SceneObject*> SceneObjectLibrary::GetAllObjects() const{
	std::vector<SceneObject*> result;
	result.reserve(allSceneObjects_.size());
	for (const auto& obj : allSceneObjects_){
		result.push_back(obj.get());
	}
	return result;
}
