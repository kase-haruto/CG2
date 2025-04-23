#include "SceneContext.h"
#include <lib/myFunc/PrimitiveDrawer.h>

SceneContext::SceneContext(){
	meshRenderer_ = std::make_unique<MeshRenderer>();
}

SceneContext::~SceneContext(){
	// 3Dオブジェクトの描画を終了
	meshRenderer_->Clear();
}
