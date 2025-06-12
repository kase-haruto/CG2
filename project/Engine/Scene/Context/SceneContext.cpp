#include "SceneContext.h"
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>
#include <Engine/Objects/3D/Actor/BaseGameObject.h>
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>
#include <Engine/Collision/CollisionManager.h>

SceneContext::SceneContext() {
	renderer_ = std::make_unique<MeshRenderer>();
	objectLibrary_ = std::make_unique<SceneObjectLibrary>();
	lightLibrary_ = std::make_unique<LightLibrary>(objectLibrary_.get());
	renderer_->SetLightLibrary(lightLibrary_.get());
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
}

void SceneContext::Clear() {
	renderer_->Clear();
	objectLibrary_->Clear();
	editorObjects_.clear();

	CollisionManager::GetInstance()->ClearColliders(); // コリジョンマネージャーのクリア
	PrimitiveDrawer::GetInstance()->ClearMesh(); // プリミティブドロワーのメッシュクリア
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