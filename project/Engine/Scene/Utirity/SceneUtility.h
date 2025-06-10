#pragma once

#include <Engine/Scene/Context/SceneContext.h>
#include <Engine/objects/3D/Actor/SceneObject.h>

#include <vector>
#include <memory>

template<typename T, typename... Args>
void CreateAndAddObject(SceneContext* context, std::unique_ptr<T>& target, Args&&... args){
	CreateAndAddObject(context->GetObjectLibrary(), target, std::forward<Args>(args)...);
}

template<typename T, typename... Args>
void CreateAndAddObject(SceneObjectLibrary* library, std::unique_ptr<T>& target, Args&&... args){
	static_assert(std::is_base_of<SceneObject, T>::value, "T must derive from SceneObject");

	target = std::make_unique<T>(std::forward<Args>(args)...);
	target->Initialize();
	if (library){
		library->AddObject(target);
	}
}
