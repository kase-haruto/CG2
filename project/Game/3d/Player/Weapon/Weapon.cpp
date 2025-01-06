#include "Weapon.h"

#include "Engine/core/Json/JsonCoordinator.h"

#include <externals/imgui/imgui.h>
#include "Engine/core/System.h"

Weapon::Weapon(const std::string& modelName)
:BaseGameObject(modelName){

	BaseGameObject::SetName("weapon");
}

//===================================================================*/
//                    main functions
//===================================================================*/
void Weapon::Initialize(){

	BaseGameObject::Initialize();

	std::string path = BaseGameObject::jsonPath + "/weapon";
	JsonCoordinator::LoadGroup(BaseGameObject::GetName(), path);

	model_->transform.translate = {0.5f,1.56f,-1.0f};
	model_->transform.rotate = {0.0f,1.56f,2.39f};
}

void Weapon::Update(){
	tipPos_ = GetCenterPos() + Vector3 {0.0f,0.7f,0.0f};

	BaseGameObject::Update();
}

void Weapon::Draw(){
	BaseGameObject::Draw();
}

//===================================================================*/
//                    gui/ui
//===================================================================*/
void Weapon::ShowGui(){

	ImGui::Text(BaseGameObject::GetName().c_str());

	if (ImGui::Button("SaveWeaponData")){
		std::string path = BaseGameObject::jsonPath + "/weapon";
		JsonCoordinator::SaveGroup(BaseGameObject::GetName(), path);
	}

	ImGui::Spacing();

	BaseGameObject::ShowGui();

	ImGui::Spacing();

	ImGui::DragFloat3("offset", &offset_.x, 0.01f);

}

void Weapon::InitializeTransform(){
	model_->transform.translate = {0.5f,1.56f,-1.0f};
	model_->transform.rotate = {0.0f,1.56f,2.39f};
	model_->transform.scale = Vector3(1.0f, 1.0f, 1.0f);
}

