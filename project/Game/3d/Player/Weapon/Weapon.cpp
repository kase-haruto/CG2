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


}

void Weapon::Update(){


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

