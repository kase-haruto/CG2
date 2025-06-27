#include "LightLibrary.h"

#include <Engine/Scene/Utirity/SceneUtility.h>

/////////////////////////////////////////////////////////////////////////////////////////
//		コンストラクタ
/////////////////////////////////////////////////////////////////////////////////////////

LightLibrary::LightLibrary(SceneObjectLibrary* library){
	// ライトを生成してSceneObjectLibraryに追加し、生ポインタだけ保持
	directionalLight_ = CreateAndAddObject<DirectionalLight>(library, "DirectionalLight");
	pointLight_ = CreateAndAddObject<PointLight>(library, "PointLight");
}

void LightLibrary::Update(){
	if (directionalLight_) directionalLight_->Update();
	if (pointLight_) pointLight_->Update();
}

void LightLibrary::Clear(){
	// SceneObjectLibrary側でクリアするときにライトも消えるので
	directionalLight_ = nullptr;
	pointLight_ = nullptr;
}

void LightLibrary::SetDirectionalLight(DirectionalLight* light){
	directionalLight_ = light;
}

void LightLibrary::SetPointLight(PointLight* light){
	pointLight_ = light;
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
