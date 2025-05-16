#include "SceneContext.h"
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>

SceneContext::SceneContext(){
	renderer_ = std::make_unique<MeshRenderer>();
	objectLibrary_ = std::make_unique<SceneObjectLibrary>();
}

SceneContext::~SceneContext(){
	// 3Dオブジェクトの描画を終了
	renderer_->Clear();
}

void SceneContext::RegisterAllToRenderer(){
	objectLibrary_->RegisterToRenderer(renderer_.get());
}