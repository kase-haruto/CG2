#include "SceneObject.h"
#include "SceneObjectManager.h"
#include <Engine/graphics/Camera/Manager/CameraManager.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <Engine/Foundation/Json/JsonUtils.h>
#include <Engine/Objects/ConfigurableObject/IConfigurable.h>

#include <externals/imgui/imgui.h>

static const char* ObjectTypeToString(ObjectType type) {
	switch (type) {
		case ObjectType::Camera:    return "Camera";
		case ObjectType::Light:     return "Light";
		case ObjectType::GameObject:return "GameObject";
		default:                    return "None";
	}
}

SceneObject::SceneObject(){

	worldTransform_.Initialize();
}

void SceneObject::ShowGui(){
}

AABB SceneObject::FallbackAABBFromTransform() const {
	Vector3 center = worldTransform_.GetWorldPosition();
	Vector3 halfScale = worldTransform_.scale * 0.5f;
	Vector3 min = center - halfScale;
	Vector3 max = center + halfScale;
	return AABB(min, max);
}

std::string SceneObject::GetObjectTypeName() const {
	return ObjectTypeToString(objectType_);
}

void SceneObject::SetName(const std::string& name, ObjectType type){
	name_ = name;
	objectType_ = type;
}

bool SceneObject::HasConfigInterface() const {
	return dynamic_cast<const IConfigurable*>(this) != nullptr;
}

