#include "MeshRenderer.h"
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/Assets/Animation/AnimationModel.h>
#include <Engine/Assets/Model/Model.h>
#include <Engine/Graphics/Camera/Manager/CameraManager.h>
#include <Engine/Graphics/Context/GraphicsGroup.h>
#include <Engine/Extensions/SkyBox/SkyBox.h>
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>
#include <Engine/objects/Transform/Transform.h>
#include <Engine/Lighting/LightLibrary.h>

// lib
#include <Engine/Foundation/Math/Matrix4x4.h>

MeshRenderer::MeshRenderer(){
}

/////////////////////////////////////////////////////////////////////////////////////////
//		描画登録
/////////////////////////////////////////////////////////////////////////////////////////
void MeshRenderer::Register(IMeshRenderable* renderable, const WorldTransform* transform){
	renderables_.emplace_back(DrawEntry {renderable, transform});
}

/////////////////////////////////////////////////////////////////////////////////////////
//		削除
/////////////////////////////////////////////////////////////////////////////////////////
void MeshRenderer::Unregister(IMeshRenderable* renderable){
	renderables_.erase(std::remove_if(renderables_.begin(), renderables_.end(),
					   [&] (const DrawEntry& entry){
						   return entry.renderable == renderable;
					   }), renderables_.end());
}

/////////////////////////////////////////////////////////////////////////////////////////
//		描画
/////////////////////////////////////////////////////////////////////////////////////////
void MeshRenderer::DrawAll() {
	std::vector<DrawEntry> staticModels;
	std::vector<DrawEntry> skinnedModels;
	DrawEntry* skyBox = nullptr;

	ID3D12GraphicsCommandList* commandList = GraphicsGroup::GetInstance()->GetCommandList().Get();

	for (const auto& entry : renderables_){
		if (dynamic_cast< AnimationModel* >(entry.renderable)){
			skinnedModels.push_back(entry);
		} else if (dynamic_cast< Model* >(entry.renderable)){
			staticModels.push_back(entry);
		} else if (dynamic_cast< SkyBox* >(entry.renderable)){
			skyBox = const_cast< DrawEntry* >(&entry);
		}
	}

	//===================================================================*/
	//                    背景オブジェクト描画
	//===================================================================*/
	// 背景
	if (skyBox){
		skyBox->renderable->Draw(*skyBox->transform);
	}

	//===================================================================*/
	//                    静的モデル描画
	//===================================================================*/
	pLightLibrary_->SetCommand(commandList, PipelineType::Object3D);
	CameraManager::SetCommand(commandList, PipelineType::Object3D);
	for (const auto& entry : staticModels){
		entry.renderable->Draw(*entry.transform);
	}
	//===================================================================*/
	//                    アニメーションモデル描画
	//===================================================================*/
	pLightLibrary_->SetCommand(commandList, PipelineType::SkinningObject3D);
	CameraManager::SetCommand(commandList, PipelineType::SkinningObject3D);
	for (const auto& entry : skinnedModels){
		entry.renderable->Draw(*entry.transform);
	}

	//===================================================================*/
	//                    プリミティブ描画
	//===================================================================*/
	GraphicsGroup::GetInstance()->SetCommand(commandList, PipelineType::Line, BlendMode::NORMAL);
	CameraManager::SetCommand(commandList, PipelineType::Line);
	PrimitiveDrawer::GetInstance()->Render();
}

void MeshRenderer::Clear() {
	renderables_.clear();
}
