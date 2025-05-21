#include "BaseScene.h"
/* ========================================================================
/*	include space
/* ===================================================================== */
#include <Engine/Graphics/Device/DxCore.h>

BaseScene::BaseScene(DxCore* dxCore):
IScene(dxCore){
	sceneContext_ = std::make_unique<SceneContext>();
}

void BaseScene::Draw(){
	auto commandList_ = pDxCore_->GetCommandList();	
	sceneContext_->GetMeshRenderer()->DrawAll();
}
