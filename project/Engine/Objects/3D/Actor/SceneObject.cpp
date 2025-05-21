#include "SceneObject.h"
#include "SceneObjectManager.h"
#include <Engine/graphics/Camera/Manager/CameraManager.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <Engine/Foundation/Json/JsonUtils.h>

#include <externals/imgui/imgui.h>
#include <externals/imgui/ImGuizmo.h>

static const char* ObjectTypeToString(ObjectType type) {
	switch (type) {
		case ObjectType::Camera:    return "Camera";
		case ObjectType::Light:     return "Light";
		case ObjectType::GameObject:return "GameObject";
		default:                    return "None";
	}
}

SceneObject::SceneObject(){

	//リストに追加

	worldTransform_.Initialize();
}

void SceneObject::ShowGui(){


}

std::string SceneObject::GetObjectTypeName() const {
	return ObjectTypeToString(objectType_);
}

void SceneObject::SetName(const std::string& name, ObjectType type){
	name_ = name;
	objectType_ = type;
}

