#include "BaseScene.h"
/* ========================================================================
/*	include space
/* ===================================================================== */
#include <Engine/Graphics/Camera/Manager/CameraManager.h>
#include <Engine/Lighting/LightManager.h>
#include <Engine/Graphics/Context/GraphicsGroup.h>
#include <Engine/Graphics/Device/DxCore.h>
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>

BaseScene::BaseScene(DxCore* dxCore):
IScene(dxCore){
	sceneContext_ = std::make_unique<SceneContext>();
}

void BaseScene::Draw(){
	auto commandList_ = pDxCore_->GetCommandList();	
	sceneContext_->GetMeshRenderer()->DrawAll();
}
