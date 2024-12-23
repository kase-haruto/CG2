#include "BaseGameObject.h"

#include "externals/imgui/imgui.h"
#include "Engine/core/Json/JsonCoordinator.h"


BaseGameObject::BaseGameObject(const std::string& modelName){

	model_ = std::make_unique<Model>(modelName);
	model_->Initialize();

}

BaseGameObject::~BaseGameObject(){}


void BaseGameObject::Initialize(){

	model_->transform.scale = {1.0f,1.0f,1.0f};
	model_->transform.rotate = {0.0f,0.0f,0.0f};
	model_->transform.translate = {0.0f,0.0f,0.0f};

}

void BaseGameObject::Update(){

	if (model_){
		model_->Update();
	}

}

void BaseGameObject::Draw(){
	
	if (model_){
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

	if (ImGui::CollapsingHeader("Transform")){
		ImGui::DragFloat3("Scale", &model_->transform.scale.x, 0.01f);
		ImGui::DragFloat3("Rotation", &model_->transform.rotate.x, 0.01f);
		ImGui::DragFloat3("Position", &model_->transform.translate.x, 0.01f);
	}

}