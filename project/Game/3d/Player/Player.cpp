#include "Player.h"

#include "Engine/collision/CollisionManager.h"
#include "lib/myFunc/MyFunc.h"
#include <externals/imgui/imgui.h>

Player::Player(const std::string& modelName)
:BaseGameObject(modelName){
	BoxCollider::Initialize(model_->transform.scale);
	CollisionManager::GetInstance()->AddCollider(this);
}

Player::~Player(){
	CollisionManager::GetInstance()->RemoveCollider(this);
}

void Player::Initialize(){
	BaseGameObject::SetName("Player");
	BaseGameObject::Initialize();
}

void Player::Update(){

	shape_.center = model_->transform.translate;
	shape_.rotate = model_->transform.rotate;
	shape_.size = model_->transform.scale + 0.1f;
	BaseGameObject::Update();
}

void Player::Draw(){
	BoxCollider::Draw();
	BaseGameObject::Draw();
}

void Player::MoveRight(){
	model_->transform.translate.x += moveSeed_*deltaTime;
}

void Player::MoveLeft(){
	model_->transform.translate.x -= moveSeed_*deltaTime;
}


void Player::ShowGui(){
	BaseGameObject::ShowGui();
}

//===================================================================*/
//                    collision
//===================================================================*/

void Player::OnCollisionEnter([[maybe_unused]] Collider* other){}

void Player::OnCollisionStay([[maybe_unused]] Collider* other){}

void Player::OnCollisionExit([[maybe_unused]] Collider* other){}

