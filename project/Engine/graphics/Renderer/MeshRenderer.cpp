#include "MeshRenderer.h"
#include <lib/myFunc/PrimitiveDrawer.h>
#include <Engine/objects/Animation/AnimationModel.h>
#include <Engine/objects/Model/Model.h>
#include <Engine/graphics/camera/CameraManager.h>
#include <lib/myMath/Matrix4x4.h>
#include <Engine/graphics/GraphicsGroup.h>
#include <Engine/physics/light/LightManager.h>

/////////////////////////////////////////////////////////////////////////////////////////
//		描画登録
/////////////////////////////////////////////////////////////////////////////////////////
void MeshRenderer::Register(IMeshRenderable* renderable){
	renderables_.push_back(renderable);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		削除
/////////////////////////////////////////////////////////////////////////////////////////
void MeshRenderer::Unregister(IMeshRenderable* renderable){
	renderables_.erase(std::remove(renderables_.begin(), renderables_.end(), renderable), renderables_.end());
}

/////////////////////////////////////////////////////////////////////////////////////////
//		描画
/////////////////////////////////////////////////////////////////////////////////////////
void MeshRenderer::DrawAll(){
	std::vector<IMeshRenderable*> staticModels;
	std::vector<IMeshRenderable*> skinnedModels;
	ID3D12GraphicsCommandList* commandList = GraphicsGroup::GetInstance()->GetCommandList().Get();
	
	for (auto* mesh : renderables_){
		if (auto* skinned = dynamic_cast< AnimationModel* >(mesh)){
			skinnedModels.push_back(skinned);
		} else{
			staticModels.push_back(mesh);
		}
	}

	//===================================================================*/
	//                    静的モデル描画
	//===================================================================*/
	// light
	LightManager::GetInstance()->SetCommand(commandList, LightType::Directional, PipelineType::Object3D);
	LightManager::GetInstance()->SetCommand(commandList, LightType::Point, PipelineType::Object3D);
	// camera
	CameraManager::SetCommand(commandList, PipelineType::Object3D);

	for (auto* mesh : staticModels){
		mesh->Draw();
	}

	//===================================================================*/
	//                    アニメーションモデル描画
	//===================================================================*/
	LightManager::GetInstance()->SetCommand(commandList, LightType::Directional, PipelineType::SkinningObject3D);
	LightManager::GetInstance()->SetCommand(commandList, LightType::Point, PipelineType::SkinningObject3D);
	CameraManager::SetCommand(commandList, PipelineType::SkinningObject3D);

	for (auto* mesh : skinnedModels){
		mesh->Draw();
	}
}

void MeshRenderer::Clear(){
	renderables_.clear();
}
