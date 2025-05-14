#include "Actor.h"

#include <Engine/core/System.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <Engine/Foundation/Json/JsonCoordinator.h>
#include <Engine/Foundation/Clock/ClockManager.h>
#include <Engine/Renderer/Mesh/IMeshRenderable.h>

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

