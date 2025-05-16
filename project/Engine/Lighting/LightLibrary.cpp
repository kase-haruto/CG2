#include "LightLibrary.h"

#include <Engine/Scene/Utirity/SceneUtility.h>

/////////////////////////////////////////////////////////////////////////////////////////
//		コンストラクタ
/////////////////////////////////////////////////////////////////////////////////////////

LightLibrary::LightLibrary(SceneObjectLibrary* library){
	CreateAndAddObject(library, directionalLight_, "DirectionalLight");
	CreateAndAddObject(library, pointLight_, "PointLight");
}

void LightLibrary::Update() {
	directionalLight_->Update();
	pointLight_->Update();
}

/////////////////////////////////////////////////////////////////////////////////////////
//		コマンドを積む(すべてのライト
/////////////////////////////////////////////////////////////////////////////////////////
void LightLibrary::SetCommand(ID3D12GraphicsCommandList* cmdList, PipelineType pipelineType){
	directionalLight_->SetCommand(cmdList, pipelineType);
	pointLight_->SetCommand(cmdList, pipelineType);
}


/////////////////////////////////////////////////////////////////////////////////////////
//		コマンドを積む(ライト別
/////////////////////////////////////////////////////////////////////////////////////////
void LightLibrary::SetCommand(ID3D12GraphicsCommandList* cmdList,
							  PipelineType pipelineType,
							  LightType lightType){
	if (lightType == LightType::Directional){
		directionalLight_->SetCommand(cmdList, pipelineType);
	} else if (lightType == LightType::Point){
		pointLight_->SetCommand(cmdList, pipelineType);
	}
}
