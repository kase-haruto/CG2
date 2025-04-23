#include "BulletContainer.h"

#include <externals/imgui/imgui.h>

BulletContainer::BulletContainer(const std::string& name
								 , std::function<void(IMeshRenderable*)> registerCB):
registerCB_(registerCB){
	bullets_.clear();
	SceneObject::SetName(name, ObjectType::GameObject);
	SceneObject::EnableGuiList();
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
	auto bullet = std::make_unique<BaseBullet>(modelName, registerCB_);
	bullet->Initialize(position,velocity);
	bullet->SetMoveSpeed(bulletSpeed_);
	bullet->SetScale(bulletScale_);
	bullets_.push_back(std::move(bullet));
}

/////////////////////////////////////////////////////////////////////////////////////////
//		削除
/////////////////////////////////////////////////////////////////////////////////////////
void BulletContainer::RemoveBullet(BaseBullet* bullet){
	// bullets_の中から指定された弾を削除
	auto it = std::remove_if(bullets_.begin(), bullets_.end(),
							 [bullet] (const std::unique_ptr<BaseBullet>& b){ return b.get() == bullet; });
}


/////////////////////////////////////////////////////////////////////////////////////////
//		gui
/////////////////////////////////////////////////////////////////////////////////////////
void BulletContainer::ShowGui(){
	ImGui::SeparatorText("bullet container");
	ImGui::DragFloat("bulletSpeed", &bulletSpeed_, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat3("bulletScale", &bulletScale_.x, 0.01f, 0.0f, 10.0f);
	DerivativeGui();
}