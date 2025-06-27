#include "SceneContext.h"
#include <Engine/Application/Effects/FxSystem.h>
#include <Engine/Collision/CollisionManager.h>
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/Objects/3D/Actor/BaseGameObject.h>
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>

SceneContext::SceneContext(){
	objectLibrary_ = std::make_unique<SceneObjectLibrary>();
	lightLibrary_ = std::make_unique<LightLibrary>(objectLibrary_.get());
	fxSystem_ = std::make_unique<FxSystem>();
}

SceneContext::~SceneContext() = default;

void SceneContext::Initialize(){
	// 必要に応じて初期化処理
}

void SceneContext::Update(){
	for (auto& obj : editorObjects_){
		obj->Update();
	}

	lightLibrary_->Update();
	fxSystem_->Update();
}

void SceneContext::Clear(){
	if (objectLibrary_){
		const auto& objects = objectLibrary_->GetAllObjects();
		for (const auto& obj : objects){
			if (onEditorObjectRemoved_){
				onEditorObjectRemoved_(obj);
			}
		}
		objectLibrary_->Clear();
	}

	editorObjects_.clear();

	fxSystem_->Clear();
	CollisionManager::GetInstance()->ClearColliders();
	PrimitiveDrawer::GetInstance()->ClearMesh();
}

void SceneContext::RemoveEditorObject(SceneObject* obj){
	objectLibrary_->RemoveObject(obj);

	auto& vec = editorObjects_;
	vec.erase(std::remove(vec.begin(), vec.end(), obj), vec.end());

	for (auto& cb : objectRemovedCallbacks_){
		cb(obj);
	}
}

std::vector<SceneObject*>& SceneContext::GetEditorObjects(){
	return editorObjects_;
}