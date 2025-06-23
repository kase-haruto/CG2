#include "SceneContext.h"
#include <Engine/Application/Effects/FxSystem.h>
#include <Engine/Collision/CollisionManager.h>
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/Objects/3D/Actor/BaseGameObject.h>
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>

SceneContext::SceneContext() {
	renderer_ = std::make_unique<MeshRenderer>();
	objectLibrary_ = std::make_unique<SceneObjectLibrary>();
	lightLibrary_ = std::make_unique<LightLibrary>(objectLibrary_.get());
	renderer_->SetLightLibrary(lightLibrary_.get());

	fxSystem_ = std::make_unique<FxSystem>();
}

SceneContext::~SceneContext() {
	renderer_->Clear();
}

void SceneContext::Initialize() {}

void SceneContext::Update() {

	for (auto& obj : editorObjects_) {
		obj->Update();
	}
	lightLibrary_->Update();

	fxSystem_->Update();
}

void SceneContext::Clear() {
	// まず ObjectLibrary のすべてのオブジェクトに削除通知を出す
	if (objectLibrary_) {
		const auto& objects = objectLibrary_->GetAllObjects();
		for (const auto& obj : objects) {
			if (onEditorObjectRemoved_) {
				onEditorObjectRemoved_(obj);
			}
		}
		objectLibrary_->Clear();
	}

	// エディタオブジェクトも破棄
	editorObjects_.clear();

	// その他のシステムのクリア
	renderer_->Clear();
	CollisionManager::GetInstance()->ClearColliders();
	PrimitiveDrawer::GetInstance()->ClearMesh();
}

void SceneContext::RegisterAllToRenderer() {
	objectLibrary_->RegisterToRenderer(renderer_.get());
}

void SceneContext::RemoveEditorObject(SceneObject* obj) {
	objectLibrary_->RemoveObject(obj);

	auto& vec = editorObjects_;
	vec.erase(std::remove_if(vec.begin(), vec.end(),
							 [obj](const std::unique_ptr<SceneObject>& o) {
		return o.get() == obj;
	}),
			  vec.end());

	for (auto& cb : objectRemovedCallbacks_)
		cb(obj);
}

std::vector<std::unique_ptr<SceneObject>>& SceneContext::GetEditorObjects() {
	return editorObjects_;
}