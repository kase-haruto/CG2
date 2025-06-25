#include "SceneObject.h"
#include "SceneObjectManager.h"
#include <Engine/graphics/Camera/Manager/CameraManager.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <Engine/Foundation/Json/JsonUtils.h>
#include <Engine/Objects/ConfigurableObject/IConfigurable.h>

#include <externals/imgui/imgui.h>

static const char* ObjectTypeToString(ObjectType type){
	switch (type){
		case ObjectType::Camera:     return "Camera";
		case ObjectType::Light:      return "Light";
		case ObjectType::GameObject: return "GameObject";
		case ObjectType::ParticleSystem: return "ParticleSystem";
		default:                     return "None";
	}
}

SceneObject::SceneObject(){
	worldTransform_.Initialize();
}

void SceneObject::ShowGui(){}

AABB SceneObject::FallbackAABBFromTransform() const{
	Vector3 center = worldTransform_.GetWorldPosition();
	Vector3 halfScale = worldTransform_.scale * 0.5f;
	Vector3 min = center - halfScale;
	Vector3 max = center + halfScale;
	return AABB(min, max);
}

std::string SceneObject::GetObjectTypeName() const{
	return ObjectTypeToString(objectType_);
}

void SceneObject::SetName(const std::string& name, ObjectType type){
	name_ = name;
	objectType_ = type;
}

bool SceneObject::HasConfigInterface() const{
	return dynamic_cast< const IConfigurable* >(this) != nullptr;
}

void SceneObject::SetParent(SceneObject* newParent){
	if (parent_ == newParent) return;

	// 現在の親からこのオブジェクトを削除
	if (parent_){
		auto& siblings = parent_->children_;
		siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
	}

	// 新しい親を設定
	parent_ = newParent;
	worldTransform_.parent = newParent ? &newParent->worldTransform_ : nullptr;

	// 新しい親の子リストに追加
	if (newParent){
		newParent->children_.push_back(this);
	}

	//// ローカル行列を再計算
	//if (newParent){
	//	Matrix4x4 parentWorldInv = Matrix4x4::Inverse(newParent->worldTransform_.matrix.world);
	//	Matrix4x4 newLocal = parentWorldInv * worldBefore;

	//	worldTransform_.translation = Matrix4x4::Translation(newLocal);
	//	worldTransform_.rotation = Quaternion::FromMatrix(newLocal);
	//}

	//// 子孫のワールド行列を再計算
	//UpdateWorldTransformRecursive();
}

void SceneObject::UpdateWorldTransformRecursive(){
	// 自身のワールド行列を更新
	worldTransform_.Update();

	// 子供たちのワールド行列を再帰的に更新
	for (SceneObject* child : children_){
		child->UpdateWorldTransformRecursive();
	}
}
