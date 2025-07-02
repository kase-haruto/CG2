#include "BulletContainer.h"
#include <Engine/Scene/Context/SceneContext.h>
#include <Engine/Scene/Utirity/SceneUtility.h>
#include <Game/3dObject/Actor/Bullet/PlayerBullet/PlayerBullet.h>

#include <externals/imgui/imgui.h>

BulletContainer::BulletContainer(const std::string& name) {
	SceneObject::SetName(name, ObjectType::GameObject);
}


void BulletContainer::Update() {
	auto* sceneLibrary = sceneContext_->GetObjectLibrary();
	for (auto& [type, bullets] : typedBullets_) {
		for (auto it = bullets.begin(); it != bullets.end(); ) {
			BaseBullet* bullet = *it;
			bullet->Update();

			if (!bullet->GetIsAlive()) {
				sceneLibrary->RemoveObject(bullet);
				it = bullets.erase(it);
			} else {
				++it;
			}
		}
	}
}

void BulletContainer::AddBullet(BulletType type,
								const Vector3& position,
								const Vector3& velocity) {
	BaseBullet* bullet = nullptr;

	if (!sceneContext_) return;
	auto* library = sceneContext_->GetObjectLibrary();

	switch (type) {
		case BulletType::Player:
			bullet = library->CreateAndAddObject<PlayerBullet>("debugCube.obj", "playerBullet");
			break;
		case BulletType::Enemy:
			//bullet = library->CreateAndAddObject<EnemyBullet>("enemyBullet.obj", "enemyBullet");
			break;
		default:
			return;
	}

	if (!bullet) return;

	bullet->ShootInitialize(position, velocity);
	bullet->SetMoveSpeed(bulletSpeed_);
	bullet->SetScale(bulletScale_);
	typedBullets_[type].push_back(bullet);
}

void BulletContainer::RemoveBullet(BaseBullet* bullet) {
	for (auto& [type, bullets] : typedBullets_) {
		bullets.remove(bullet);
	}
}

const std::list<BaseBullet*>& BulletContainer::GetBullets(BulletType type) const {
	static const std::list<BaseBullet*> empty;
	auto it = typedBullets_.find(type);
	if (it != typedBullets_.end()) {
		return it->second;
	}
	return empty;
}

void BulletContainer::ShowGui() {
	ImGui::SeparatorText("bullet container");
	DerivativeGui();

	for (const auto& [type, bullets] : typedBullets_) {
		ImGui::Text("Type %d : %d bullets", static_cast<int>(type), static_cast<int>(bullets.size()));
	}
}

void BulletContainer::DerivativeGui() {
	ImGui::DragFloat("bulletSpeed", &bulletSpeed_, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat3("bulletScale", &bulletScale_.x, 0.01f, 0.0f, 10.0f);
}
