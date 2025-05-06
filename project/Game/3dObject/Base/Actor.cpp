#include "Actor.h"

#include "Engine/core/System.h"
#include "lib/myFunc/MyFunc.h"
#include "Engine/core/Json/JsonCoordinator.h"
#include "Engine/core/Clock/ClockManager.h"
#include "Engine/objects/Mesh/IMeshRenderable.h"

Actor::Actor(const std::string& modelName,
			 std::optional<std::string> objectName,
			 std::function<void(IMeshRenderable*)> registerCB) :
	BaseGameObject::BaseGameObject(modelName, objectName, registerCB) {

}

void Actor::Initialize() {

	//JsonCoordinator::RegisterItem(name_, "MoveSpeed", moveSpeed_);

}

void Actor::Update() {
	//const float deltaTime = ClockManager::GetInstance()->GetDeltaTime();

	//// 最終的な位置更新
	//model_->worldTransform_.translation += velocity_ * deltaTime;
}

