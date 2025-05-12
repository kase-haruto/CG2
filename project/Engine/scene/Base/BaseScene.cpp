#include "BaseScene.h"
#include "Engine/graphics/camera/CameraManager.h"
#include "Engine/physics/light/LightManager.h"
#include "Engine/graphics/GraphicsGroup.h"
#include <Engine/core/DirectX/DxCore.h>

#include <lib/myFunc/PrimitiveDrawer.h>

BaseScene::BaseScene(DxCore* dxCore):
IScene(dxCore){
	sceneContext_ = std::make_unique<SceneContext>();
}

void BaseScene::Draw(){
	auto commandList_ = pDxCore_->GetCommandList();	
	sceneContext_->meshRenderer_->DrawAll();

	
}
