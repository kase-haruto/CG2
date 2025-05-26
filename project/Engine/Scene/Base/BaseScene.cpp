#include "BaseScene.h"
/* ========================================================================
/*	include space
/* ===================================================================== */
#include <Engine/Graphics/Device/DxCore.h>

BaseScene::BaseScene(DxCore* dxCore):
IScene(dxCore){
	sceneContext_ = std::make_unique<SceneContext>();
}

void BaseScene::Draw(ID3D12GraphicsCommandList* cmdList){
	sceneContext_->GetMeshRenderer()->DrawAll(cmdList);
}
