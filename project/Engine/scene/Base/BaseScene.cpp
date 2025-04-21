#include "BaseScene.h"
#include "Engine/graphics/camera/CameraManager.h"
#include "Engine/physics/light/LightManager.h"

#include <Engine/core/DirectX/DxCore.h>

#include <lib/myFunc/PrimitiveDrawer.h>

BaseScene::BaseScene(DxCore* dxCore):
IScene(dxCore){
	sceneContext_ = std::make_unique<SceneContext>();
}

void BaseScene::Draw(){
	auto commandList_ = pDxCore_->GetCommandList();
	// light
	LightManager::GetInstance()->SetCommand(commandList_, LightType::Directional, PipelineType::Object3D);
	LightManager::GetInstance()->SetCommand(commandList_, LightType::Point, PipelineType::Object3D);
	// camera
	CameraManager::SetCommand(commandList_, PipelineType::Object3D);

	// 3Dオブジェクトの描画
	sceneContext_->meshRenderer_->DrawAll();

}
