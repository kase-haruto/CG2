#include "BaseGameObject.h"

#include "externals/imgui/imgui.h"
#include "Engine/core/Json/JsonCoordinator.h"


BaseGameObject::BaseGameObject(const std::string& modelName){

	model_ = std::make_unique<Model>(modelName);
	model_->Initialize();

}

BaseGameObject::~BaseGameObject(){}


void BaseGameObject::Initialize(){

	// JsonCoordinator にプロパティを登録
	JsonCoordinator::RegisterItem(name_, "Scale", model_->transform.scale);
	JsonCoordinator::RegisterItem(name_, "Rotate", model_->transform.rotate);
	JsonCoordinator::RegisterItem(name_, "Translate", model_->transform.translate);

	jsonPath = "gameobject/" + GetName();
	JsonCoordinator::Load(name_, jsonPath);

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
	// JsonCoordinator を利用して保存・ロードボタンを表示
	if (ImGui::Button("Save")){
		JsonCoordinator::Save(name_, jsonPath);
	}

	if (ImGui::CollapsingHeader("Transform")){
		JsonCoordinator::RenderGroupUI(name_);
	}

}