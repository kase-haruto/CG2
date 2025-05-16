#pragma once

#include <Engine/Scene/Context/SceneContext.h>
#include <Engine/objects/3D/Actor/SceneObject.h>

#include <vector>
#include <memory>

template<typename T, typename... Args>
void CreateAndAddObject(SceneContext* context, std::unique_ptr<T>& target, Args&&... args){
	target = std::make_unique<T>(std::forward<Args>(args)...);
	target->Initialize();
	context->GetObjectLibrary()->AddObject(target.get());
}