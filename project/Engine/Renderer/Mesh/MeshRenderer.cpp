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
#include <Engine/Graphics/Pipeline/Presets/PipelinePresets.h>

// lib
#include <Engine/Foundation/Math/Matrix4x4.h>

MeshRenderer::MeshRenderer(){

}

/////////////////////////////////////////////////////////////////////////////////////////
//		描画登録
/////////////////////////////////////////////////////////////////////////////////////////
void MeshRenderer::Register(IMeshRenderable* renderable, const WorldTransform* transform){
	renderables_.emplace_back(DrawEntry {renderable, transform});
	renderable->AddDeathListener(this);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		削除
/////////////////////////////////////////////////////////////////////////////////////////
void MeshRenderer::Unregister(IMeshRenderable* renderable){
	renderables_.erase(
		std::remove_if(renderables_.begin(), renderables_.end(),
					   [&](const DrawEntry& entry) { return entry.renderable == renderable; }),
		renderables_.end()
	);
}

void MeshRenderer::OnRenderableDestroyed(IMeshRenderable* obj) {
	Unregister(obj);
}


/////////////////////////////////////////////////////////////////////////////////////////
//		描画
/////////////////////////////////////////////////////////////////////////////////////////
void MeshRenderer::DrawAll(ID3D12GraphicsCommandList* cmdList) {
	std::vector<DrawEntry> staticModels;
	std::vector<DrawEntry> skinnedModels;


	for (const auto& entry : renderables_){
		if (dynamic_cast< AnimationModel* >(entry.renderable)){
			skinnedModels.push_back(entry);
		} else if (dynamic_cast< Model* >(entry.renderable)){
			staticModels.push_back(entry);
		}
	}

	//===================================================================*/
	//                    背景オブジェクト描画
	//===================================================================*/
	cmdList->SetGraphicsRootSignature(GraphicsGroup::GetInstance()->GetRootSignature(PipelineType::Skybox).Get());
	// 背景
	if (skyBox_){
		skyBox_->Draw(skyBox_->GetWorldTransform());
	}

	//===================================================================*/
	//                    静的モデル描画
	//===================================================================*/
	//CameraManager::SetCommand(cmdList, PipelineType::Object3D);
	//pLightLibrary_->SetCommand(cmdList, PipelineType::Object3D);
	//// 静的モデルの描画
	//for (auto& staticModel:staticModels) {
	//	staticModel.renderable->Draw(*staticModel.transform);
	//}

	DrawGroup(cmdList,staticModels, PipelineType::Object3D);

	//===================================================================*/
	//                    アニメーションモデル描画
	//===================================================================*/
	// アニメーションモデルの描画
	for (auto& animationModel : skinnedModels) {
		BlendMode mode = animationModel.renderable->GetBlendMode();
		GraphicsPipelineDesc desc = PipelinePresets::MakeSkinningObject3D(mode);
		pipelineService_->SetCommand(desc, cmdList);
		CameraManager::SetCommand(cmdList, PipelineType::SkinningObject3D);
		pLightLibrary_->SetCommand(cmdList, PipelineType::SkinningObject3D);
		animationModel.renderable->Draw(*animationModel.transform);
	}


	//===================================================================*/
	//                    プリミティブ描画
	//===================================================================*/
	GraphicsGroup::GetInstance()->SetCommand(cmdList, PipelineType::Line, BlendMode::NORMAL);
	CameraManager::SetCommand(cmdList, PipelineType::Line);
	PrimitiveDrawer::GetInstance()->Render();
}

void MeshRenderer::DrawGroup(ID3D12GraphicsCommandList* cmdList,const std::vector<DrawEntry>& entries, PipelineType type) {
	if (entries.empty()) return;

	for (const auto& entry : entries) {
		// 各オブジェクトの BlendMode を取得して PSO を構築
		BlendMode mode = entry.renderable->GetBlendMode();
		GraphicsPipelineDesc desc = PipelinePresets::MakeObject3D(mode);
		pipelineService_->SetCommand(desc, cmdList);
		CameraManager::SetCommand(cmdList, type);
		pLightLibrary_->SetCommand(cmdList, type);

		entry.renderable->Draw(*entry.transform);
	}
}

void MeshRenderer::Clear() {
	renderables_.clear();
}
