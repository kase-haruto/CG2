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

	model_->transform.translate = INITIAL_TRANSLATE;
	model_->transform.rotate = INITIAL_ROTATE;
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

void Weapon::InitializeTransform(){
	// 現在の変換値から初期値に向かって線形補間
	model_->transform.translate = Vector3::Lerp(model_->transform.translate, INITIAL_TRANSLATE, 0.2f);
	model_->transform.rotate = Vector3::Lerp(model_->transform.rotate, INITIAL_ROTATE, 0.2f);
	model_->transform.scale = Vector3::Lerp(model_->transform.scale, INITIAL_SCALE, 0.2f);

}

Vector3 Weapon::ComputeTipWorldPosition() const{
	// ローカル先端座標を取得
	Vector3 tipLocalPosition = tipPos_;
	// ワールド行列を用いてローカル座標をワールド空間に変換
	Vector3 tipWorldPosition = Vector3::Transform(tipLocalPosition, model_->worldMatrix);
	return tipWorldPosition;
}