#include "SceneContext.h"
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>
#include <Engine/Objects/3D/Actor/BaseGameObject.h>

SceneContext::SceneContext(){
	renderer_ = std::make_unique<MeshRenderer>();
	objectLibrary_ = std::make_unique<SceneObjectLibrary>();
	lightLibrary_ = std::make_unique<LightLibrary>(objectLibrary_.get());
	renderer_->SetLightLibrary(lightLibrary_.get());
}

SceneContext::~SceneContext(){
	renderer_->Clear();
}

void SceneContext::Initialize(){}

void SceneContext::Update(){
	for (auto& obj : editorObjects_){
		obj->Update();
	}
	lightLibrary_->Update();
}

void SceneContext::RegisterAllToRenderer(){
	objectLibrary_->RegisterToRenderer(renderer_.get());
}

void SceneContext::RemoveEditorObject(const std::shared_ptr<SceneObject>& obj){
	auto it = std::remove(editorObjects_.begin(), editorObjects_.end(), obj);
	if (it != editorObjects_.end()){
		editorObjects_.erase(it, editorObjects_.end());

		// コールバック通知
		for (auto& cb : objectRemovedCallbacks_){
			cb(obj.get());
		}
	}
}

std::vector<std::shared_ptr<SceneObject>>& SceneContext::GetEditorObjects(){
	return editorObjects_;
}