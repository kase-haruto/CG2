#include "Player.h"

#include "Engine/collision/CollisionManager.h"
#include "lib/myFunc/MyFunc.h"

#include "Engine/graphics/camera/CameraManager.h"
#include "Engine/core/Json/JsonCoordinator.h"

//< state
#include "PlayerState/PlayerState_Stay.h"
#include "PlayerState/PlayerState_Jog.h"
#include "PlayerState/PlayerState_Dush.h"

#include <externals/imgui/imgui.h>
#include "Engine/core/Input.h"
#include "Engine/core/System.h"

Player::Player(const std::string& modelName)
	:Character(modelName){

	CollisionManager::GetInstance()->AddCollider(this);
	//オブジェクトの名前を設定
	BaseGameObject::SetName("Player");
	Collider::SetName("Player");

}

Player::~Player(){

	CollisionManager::GetInstance()->RemoveCollider(this);

}

//===================================================================*/
//                    main functions
//===================================================================*/
void Player::Initialize(){

	BaseGameObject::Initialize();

	//colliderの設定
	BoxCollider::Initialize(model_->transform.scale);
	Collider::type_ = ColliderType::Type_Player;
	Collider::targetType_ = ColliderType::Type_Enemy;

	model_->transform.translate.z = -15.0f;
	model_->transform.scale = {0.8f, 0.8f, 0.8f};
	moveSpeed_ = 10.0f;

	attackController_ = std::make_unique<PlayerAttackController>();
	attackController_->SetPlayer(this);

	JsonCoordinator::LoadGroup(BaseGameObject::GetName(), BaseGameObject::jsonPath);

	// 状態クラスの初期化
	pState_ = std::make_unique<PlayerState_Stay>(this);
	pState_->Initialize();

	// 武器の初期化
	weapon_ = std::make_unique<Weapon>("weapon.obj");
	weapon_->Initialize();
	weapon_->GetModel()->parent_ = &model_->transform;


}

void Player::Update(){
	// 攻撃入力の検出
	if (Input::TriggerGamepadButton(PAD_BUTTON::X)){
		attackController_->HandleAttackInput();
	}

	//状態の切り替え


	if (Input::IsLeftStickMoved()){
		// 移動入力がある場合
		TransitionState(PlayerState::Jog);
		// ダッシュ
		if (Input::PushGamepadButton(PAD_BUTTON::RB)){
			TransitionState(PlayerState::Dush);
		}

	} else{
		// 移動入力がない場合ジャンプしていなければステイ
		if (state_ != PlayerState::Jump){
			TransitionState(PlayerState::Stay);
		}
	}

	// ジャンプ
	if (!onGround_){
		if (Input::TriggerGamepadButton(PAD_BUTTON::A)){
			Vector3 currentVelocity = GetVelocity();
			Vector3 jumpVel = {currentVelocity.x, jumpPower_, currentVelocity.z};
			SetVelocity(jumpVel);
		}
	}

	weapon_->Update();

	// 状態クラスの更新
	pState_->Update();

	shape_.center = GetCenterPos();
	shape_.rotate = model_->transform.rotate;

	// 攻撃管理クラスの更新
	attackController_->Update();

	BaseGameObject::Update();
	Character::Update();
}

void Player::Draw(){

	// 状態クラスの描画
	pState_->Draw();

	weapon_->Draw();

	BoxCollider::Draw();
	BaseGameObject::Draw();

	// 攻撃管理クラスの描画
	attackController_->Draw();

}



void Player::TransitionState(PlayerState nextState){
	// 状態クラスのクリーンアップ
	pState_->Cleanup();

	switch (nextState){
		case PlayerState::Stay:
			pState_ = std::make_unique<PlayerState_Stay>(this);
			break;
		case PlayerState::Jog:
			pState_ = std::make_unique<PlayerState_Jog>(this);
			break;
		case PlayerState::Dush:
			pState_ = std::make_unique<PlayerState_Dush>(this);
			break;
		case PlayerState::Dead:
			break;
	}
	// 状態クラスの初期化
	pState_->Initialize();
}

std::string Player::GetAttackInput(){
	if (Input::TriggerGamepadButton(PAD_BUTTON::X)){
		return "XButtonAttack"; // プレースホルダー。実際にはHandleAttackInputを使用
	}
	return "";
}


//===================================================================*/
//                    collision
//===================================================================*/
void Player::OnCollisionEnter([[maybe_unused]] Collider* other){

	//* 衝突相手がtargetType_に含まれていなければreturn
	if ((other->GetType() & Collider::GetTargetType()) != ColliderType::Type_None){


		//////////////////////////////////////////////////////////////////
		//				敵と衝突
		//////////////////////////////////////////////////////////////////
		if (other->GetType() == ColliderType::Type_Enemy){



		}

	}

}

void Player::OnCollisionStay([[maybe_unused]] Collider* other){}

void Player::OnCollisionExit([[maybe_unused]] Collider* other){}

//===================================================================*/
//                    imgui/ui
//===================================================================*/
void Player::ShowGui(){

	SceneObject::ShowGui();

	ImGui::Spacing(); // 少しスペースを追加

	// Saveボタン
	if (ImGui::Button("Save")){
		JsonCoordinator::SaveGroup(BaseGameObject::GetName(), BaseGameObject::jsonPath);
	}


	// BaseGameObjectのGUI表示
	BaseGameObject::ShowGui();


	// BoxColliderのGUI表示
	BoxCollider::ShowGui();

	// AttackControllerのGUI表示
	attackController_->ShowGui();

}

PlayerAttackController* Player::GetAttackController(){
	return attackController_.get();
}

const EulerTransform& Player::GetTransform() const{
	return model_->transform;
}

//===================================================================*/
//                    getter
//===================================================================*/
const Vector3 Player::GetCenterPos()const{
	const Vector3 offset = {0.0f, 1.0f, 0.0f};
	Vector3 worldPos;
	if (model_){
		worldPos = Vector3::Transform(offset, model_->worldMatrix);
	}
	return worldPos;
}

const Matrix4x4 Player::GetWorldMatrix() const{
	return model_->worldMatrix;
}

Vector3 Player::GetForward() const{

	Matrix4x4 rotationMatrix = EulerToMatrix(model_->transform.rotate);

	Vector3 forward = {
		rotationMatrix.m[2][0],
		rotationMatrix.m[2][1],
		rotationMatrix.m[2][2]
	};

	return forward.Normalize();

}