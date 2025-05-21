#include "SceneContext.h"
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>

SceneContext::SceneContext(){
	renderer_ = std::make_unique<MeshRenderer>();
	objectLibrary_ = std::make_unique<SceneObjectLibrary>();
	lightLibrary_ = std::make_unique<LightLibrary>(objectLibrary_.get());
	renderer_->SetLightLibrary(lightLibrary_.get());
}

SceneContext::~SceneContext(){
	// 3Dオブジェクトの描画を終了
	renderer_->Clear();
}

void SceneContext::Update() {
	lightLibrary_->Update();
}

void SceneContext::RegisterAllToRenderer(){
	objectLibrary_->RegisterToRenderer(renderer_.get());
}