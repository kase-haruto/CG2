#include "SceneContext.h"
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>

SceneContext::SceneContext(){
	meshRenderer_ = std::make_unique<MeshRenderer>();
}

SceneContext::~SceneContext(){
	// 3Dオブジェクトの描画を終了
	meshRenderer_->Clear();
}
