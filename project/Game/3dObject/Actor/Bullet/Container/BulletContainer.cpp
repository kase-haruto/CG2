#include "BulletContainer.h"
#include <Engine/Scene/Utirity/SceneUtility.h>
#include <Engine/Scene/Context/SceneContext.h>
#include <externals/imgui/imgui.h>

BulletContainer::BulletContainer(const std::string& name){
	bullets_.clear();
	SceneObject::SetName(name, ObjectType::GameObject);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		更新
/////////////////////////////////////////////////////////////////////////////////////////
void BulletContainer::Update(){
	for (auto it = bullets_.begin(); it != bullets_.end(); ){
		(*it)->Update();
		if (!(*it)->GetIsAlive()){
			it = bullets_.erase(it);
		} else{
			++it;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		追加
/////////////////////////////////////////////////////////////////////////////////////////
void BulletContainer::AddBullet(const std::string& modelName,
								const Vector3& position,
								const Vector3& velocity){
	BaseBullet* bullet = nullptr;

	if (sceneContext_){
		bullet = sceneContext_->GetObjectLibrary()->CreateAndAddObject<BaseBullet>(modelName, "bullet");
	} else{
		auto tmp = std::make_unique<BaseBullet>(modelName, "bullet");
		bullet = tmp.get();
	}

	bullet->ShootInitialize(position, velocity);
	bullet->SetMoveSpeed(bulletSpeed_);
	bullet->SetScale(bulletScale_);

	bullets_.push_back(bullet);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		削除
/////////////////////////////////////////////////////////////////////////////////////////
void BulletContainer::RemoveBullet(BaseBullet* bullet){
	bullets_.remove(bullet);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		gui
/////////////////////////////////////////////////////////////////////////////////////////
void BulletContainer::ShowGui(){
	ImGui::SeparatorText("bullet container");

	DerivativeGui();
}

void BulletContainer::DerivativeGui(){
	ImGui::DragFloat("bulletSpeed", &bulletSpeed_, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat3("bulletScale", &bulletScale_.x, 0.01f, 0.0f, 10.0f);
}
