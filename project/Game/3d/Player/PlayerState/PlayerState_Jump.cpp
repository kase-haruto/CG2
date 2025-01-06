#include "PlayerState_Jump.h"

#include "../Player.h"
#include "Engine/core/Input.h"
#include "Engine/graphics/camera/CameraManager.h"
#include "Engine/core/System.h"

//< lib >
#include "lib/myfunc/MyFunc.h"

//< externals >
#include <externals/imgui/imgui.h>


PlayerState_Jump::PlayerState_Jump(Player* player)
	:PlayerState_Base(player){}

//*===================================================================*/
//                    mainFunc*/
/*===================================================================*/
void PlayerState_Jump::Initialize(){}

void PlayerState_Jump::Update(){
	if (!pPlayer_->IsAttacking()){
		Jump();
		Move();
	}
}

void PlayerState_Jump::Draw(){}

void PlayerState_Jump::Cleanup(){}

//*===================================================================*/
//                    helperfunc	*/
/*===================================================================*/
void PlayerState_Jump::Jump(){
	if (!hasJumped_){
		Vector3 currentVelocity = pPlayer_->GetVelocity();
		Vector3 jumpVel = {currentVelocity.x, jumpPower, currentVelocity.z};
		pPlayer_->SetVelocity(jumpVel);
		hasJumped_ = true;
	}
}

void PlayerState_Jump::Move(){
	Vector3 moveDirection = {Input::GetLeftStick().x,0.0f,Input::GetLeftStick().y};
	moveVelocity_ = moveDirection * moveSpeed_;
	Vector3 rotate = CameraManager::GetInstance()->GetFollowRotate();
	Matrix4x4 matRotateY = MakeRotateYMatrix(rotate.y);
	Matrix4x4 matRotateZ = MakeRotateZMatrix(rotate.z);
	Matrix4x4 matRotate = Matrix4x4::Multiply(matRotateY, matRotateZ);
	moveVelocity_ = Vector3::Transform(moveVelocity_, matRotate);

	pPlayer_->GetModel()->transform.translate += moveVelocity_ * System::GetDeltaTime();

	float horizontalDistance = sqrtf(moveVelocity_.x * moveVelocity_.x + moveVelocity_.z * moveVelocity_.z);
	pPlayer_->GetModel()->transform.rotate.x = std::atan2(-moveVelocity_.y, horizontalDistance);

	// 目標角度を計算し、補間を適用
	targetAngle_ = std::atan2(moveVelocity_.x, moveVelocity_.z);
	pPlayer_->GetModel()->transform.rotate.y = LerpShortAngle(pPlayer_->GetModel()->transform.rotate.y, targetAngle_, 0.1f);

}

//*===================================================================*/
//                    ui表示	*/
//*===================================================================*/
void PlayerState_Jump::ShowGui(){

	ImGui::Text("PlayerState_jump");

	ImGui::Separator();

	ImGui::DragFloat("animationSpeed", &animationSpeed_, 0.01f);
}

