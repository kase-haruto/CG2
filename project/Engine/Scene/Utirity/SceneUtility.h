#pragma once

#include <Engine/Scene/Context/SceneContext.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>

#include <vector>
#include <memory>

template<typename T, typename... Args>
T* CreateAndAddObject(SceneContext* context, Args&&... args){
	return CreateAndAddObject<T>(context->GetObjectLibrary(), std::forward<Args>(args)...);
}

template<typename T, typename... Args>
T* CreateAndAddObject(SceneObjectLibrary* library, Args&&... args){
	static_assert(std::is_base_of<SceneObject, T>::value, "T must derive from SceneObject");

	auto obj = std::make_unique<T>(std::forward<Args>(args)...);
	obj->Initialize();
	T* raw = obj.get();

	if (library){
		library->AddObject(std::move(obj)); // 所有権を渡す
	}
	return raw;
}
