#include "BaseGameObject.h"

#include "externals/imgui/imgui.h"
#include "Engine/core/Json/JsonCoordinator.h"


BaseGameObject::BaseGameObject(const std::string& modelName){

	auto dotPos = modelName.find_last_of('.');
	if (dotPos != std::string::npos){
		std::string extension = modelName.substr(dotPos);

		// obj
		if (extension == ".obj"){
			objectModelType_ = ObjectModelType::ModelType_Static;
			model_ = std::make_unique<Model>(modelName);
			model_->transform.Initialize();
		}
		// gltf
		else if (extension == ".gltf"){
			objectModelType_ = ObjectModelType::ModelType_Animation;
			model_ = std::make_unique<AnimationModel>(modelName);
			model_->transform.Initialize();
		}
		// その他の拡張子の場合はここに追加
		else{
			// Handle other extensions or set a default type
			objectModelType_ = ObjectModelType::ModelType_Unknown;
		}

	}
}

BaseGameObject::~BaseGameObject(){}


void BaseGameObject::Initialize(){}

void BaseGameObject::Update(){

	if (objectModelType_ != ObjectModelType::ModelType_Unknown){

		// staticModel
		if (objectModelType_ == ObjectModelType::ModelType_Static){
			StaticModelUpdate();
		}

		// animationModel
		else if (objectModelType_ == ObjectModelType::ModelType_Animation){
			AnimationModelUpdate();
		}

	}

}

void BaseGameObject::AnimationModelUpdate(){
	if (model_){
		model_->AnimationUpdate();
	}
}

void BaseGameObject::StaticModelUpdate(){
	if (model_){
		model_->Update();
	}
}

void BaseGameObject::Draw(){

	if (objectModelType_ != ObjectModelType::ModelType_Unknown){

		model_->Draw();

	}

}


//===================================================================*/
//                   getter/setter
//===================================================================*/
void BaseGameObject::SetName(const std::string& name){
	SceneObject::SetName(name, ObjectType::GameObject);
}



//===================================================================*/
//                    imgui/ui
//===================================================================*/
void BaseGameObject::ShowGui(){
	ImGui::Spacing();
	
	model_->ShowImGuiInterface();

	
	
	ImGui::Spacing();
}