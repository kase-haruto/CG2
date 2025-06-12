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

void LightLibrary::Clear() {
	/*directionalLight_.reset();
	pointLight_.reset();*/
}

void LightLibrary::SetDirectionalLight(std::unique_ptr<DirectionalLight> light) {
	if (light) {
		directionalLight_ = std::move(light);
	} else {
		directionalLight_ = std::make_unique<DirectionalLight>("DefaultDirectionalLight");
	}
}

void LightLibrary::SetPointLight(std::unique_ptr<PointLight> light) {
	if (light) {
		pointLight_ = std::move(light);
	} else {
		pointLight_ = std::make_unique<PointLight>("DefaultPointLight");
	}
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
