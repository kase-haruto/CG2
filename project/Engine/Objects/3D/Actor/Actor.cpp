#include "Actor.h"

Actor::Actor(const std::string& modelName,
			 std::optional<std::string> objectName) :
	BaseGameObject::BaseGameObject(modelName, objectName) {

}

void Actor::Initialize() {

	//JsonCoordinator::RegisterItem(name_, "MoveSpeed", moveSpeed_);

}

void Actor::Update() {
	//const float deltaTime = ClockManager::GetInstance()->GetDeltaTime();

	//// 最終的な位置更新
	//model_->worldTransform_.translation += velocity_ * deltaTime;
}

