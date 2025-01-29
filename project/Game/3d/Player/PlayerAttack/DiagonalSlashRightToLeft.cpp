#include "DiagonalSlashRightToLeft.h"
#include "Engine/core/System.h"
#include "lib/myFunc/MathFunc.h"
#include "Engine/core/Json/JsonCoordinator.h"
#include "../Player.h"

#include "Engine/core/Audio/Audio.h"
#include "Engine/core/Input.h"
#include "Engine/graphics/camera/CameraManager.h"

#include "Engine/core/Clock/ClockManager.h"

#include "lib/myFunc/MyFunc.h"
#include "Engine/graphics/GraphicsGroup.h"
#include <externals/imgui/imgui.h>

DiagonalSlashRightToLeft::DiagonalSlashRightToLeft(const std::string& attackName)
	: IPlayerAttack(attackName),
	animationTime_(0.0f),
	animationSpeed_(2.5f) // 必要に応じて調整
{
	SetupDefaultControlPoints();
}

//////////////////////////////////////////////////////////////////////////
//						main functions
//////////////////////////////////////////////////////////////////////////
void DiagonalSlashRightToLeft::Initialize(){
	offset_ = 2.0f;
	shape_.center = center_ + pPlayer_->GetForward() * offset_;

	IPlayerAttack::Initialize();

	isAttacking_ = true;
	animationTime_ = 0.0f;

	Vector3 weaponRotate {1.4f,0.0f,0.0f};
	weapon_->SetRotate(weaponRotate);

	damage_ = 235; // ダメージ値を設定


	//===========================================
   // ここで SwordTrail を初期化
   //===========================================
	swordTrail_.Initialize();

	Audio::Play("attack.mp3", false);

	// 初期回転を設定し、初期回転を保存
	initialRotate_ = {0.0f, 0.48f, -0.59f};
	weapon_->SetRotate(initialRotate_);

	pPlayer_->SetIsAttacking(true);
}

void DiagonalSlashRightToLeft::Execution(){
	Initialize();
}

void DiagonalSlashRightToLeft::Update(){
	if (!isAttacking_) return;

	// アニメーション時間を更新
	animationTime_ += animationSpeed_ * ClockManager::GetInstance()->GetDeltaTime();
	if (animationTime_ > 1.0f){
		animationTime_ = 1.0f;
		isAttacking_ = false;
		Cleanup();
	}

	// Catmull-Rom 補間
	currentPosition_ = CatmullRomPosition(controlPoints_, animationTime_);

	// 武器の位置を更新
	weapon_->SetPosition(currentPosition_);
	// 武器の回転をアニメーション進行度に基づいて補間
   // 線形補間 (Lerp) を使用
	Vector3 currentRotate = Vector3::Lerp(initialRotate_, targetRotate_, animationTime_);
	weapon_->SetRotate(currentRotate);
	Vector3 tip = weapon_->ComputeTipWorldPosition();
	Vector3 base = weapon_->GetBasePos();

	if (!hasPrevFrame_){
		// 最初のフレームは軌跡を追加しない
		prevTip_ = tip;
		prevBase_ = base;
		hasPrevFrame_ = true;
	} else{
		// 2フレーム目以降は前フレームとの連続で軌跡を追加
		swordTrail_.AddSegment(tip, base);

		// 次フレーム用に記憶
		prevTip_ = tip;
		prevBase_ = base;
	}

	swordTrail_.Update(ClockManager::GetInstance()->GetDeltaTime());

	Vector3 moveDirection = {Input::GetLeftStick().x, 0.0f, Input::GetLeftStick().y};
	moveVelocity_ = moveDirection * 2.0f; // jogSpeed_ は移動速度

	// カメラの回転を考慮した移動
	Vector3 cameraRotate = CameraManager::GetInstance()->GetFollowRotate();
	Matrix4x4 matRotateY = MakeRotateYMatrix(cameraRotate.y);
	Matrix4x4 matRotateZ = MakeRotateZMatrix(cameraRotate.z);
	Matrix4x4 matRotate = Matrix4x4::Multiply(matRotateY, matRotateZ);
	moveVelocity_ = Vector3::Transform(moveVelocity_, matRotate);

	// プレイヤー位置を更新
	pPlayer_->GetModel()->transform.translate += moveVelocity_ * ClockManager::GetInstance()->GetDeltaTime();

	float horizontalDistance = sqrtf(moveVelocity_.x * moveVelocity_.x + moveVelocity_.z * moveVelocity_.z);
	pPlayer_->GetModel()->transform.rotate.x = std::atan2(-moveVelocity_.y, horizontalDistance);

	if (Input::IsLeftStickMoved()){
		float targetAngle_ = std::atan2(moveVelocity_.x, moveVelocity_.z);
		pPlayer_->GetModel()->transform.rotate.y =
			LerpShortAngle(pPlayer_->GetModel()->transform.rotate.y, targetAngle_, 0.1f);

	}

	// 攻撃形状を更新
	shape_.center = center_ + pPlayer_->GetForward() * offset_;
	shape_.rotate = pPlayer_->GetRotate();

	// 攻撃形状を更新
	//攻撃範囲をプレイヤーの周りに設定
	shape_.center = center_ + pPlayer_->GetForward() * offset_;
	shape_.rotate = pPlayer_->GetRotate();
}

void DiagonalSlashRightToLeft::Draw(){
	// 攻撃形状(コリジョン可視化)の描画
	BoxCollider::Draw();

	// トレイルの描画
	swordTrail_.Draw();
}

void DiagonalSlashRightToLeft::Cleanup(){
	// クリーンアップ処理
	isAttacking_ = false;
	animationTime_ = 0.0f;
	animationSpeed_ = 1.0f;
	isActive_ = false;
	pPlayer_->SetIsAttacking(false);
}

//////////////////////////////////////////////////////////////////////////
//						helper functions
//////////////////////////////////////////////////////////////////////////
void DiagonalSlashRightToLeft::SetupDefaultControlPoints(){
	// デフォルトの制御点を設定
	controlPoints_.emplace_back(Vector3(-0.5f, 0.0f, 0.0f)); // p0
	controlPoints_.emplace_back(Vector3(0.0f, 0.5f, 0.0f));  // p1
	controlPoints_.emplace_back(Vector3(0.5f, 0.0f, 0.0f));  // p2
	controlPoints_.emplace_back(Vector3(0.0f, -0.5f, 0.0f)); // p3
}

std::unique_ptr<IPlayerAttack> DiagonalSlashRightToLeft::Clone() const{
	auto clonedAttack = std::make_unique<DiagonalSlashRightToLeft>(*this);
	clonedAttack->SetIsActive(false); // クローン時は一旦非アクティブ
	return clonedAttack;
}


//////////////////////////////////////////////////////////////////////////
//						imgui/ui
//////////////////////////////////////////////////////////////////////////
void DiagonalSlashRightToLeft::ShowGui(){
	ImGui::Begin(("Edit " + GetName()).c_str());

	// 制御点の編集
	for (size_t i = 0; i < controlPoints_.size(); ++i){
		std::string label = "Control Point " + std::to_string(i);
		ImGui::DragFloat3(label.c_str(), &controlPoints_[i].x, 0.1f);
	}

	// 制御点の追加・削除
	if (ImGui::Button("Add Control Point")){
		controlPoints_.emplace_back(Vector3(0.0f, 0.0f, 0.0f));
	}

	if (ImGui::Button("Remove Last Control Point") && !controlPoints_.empty()){
		controlPoints_.pop_back();
	}

	ImGui::Separator();

	// 軌道の可視化ボタン
	if (ImGui::Button("Visualize Trajectory")){
		// ここに軌道の可視化処理を実装（オプション）
	}

	// 軌道の保存・読み込みボタン
	if (ImGui::Button("Save Control Points")){
		SaveControlPoints(GetName() + "_control_points.json");
	}

	if (ImGui::Button("Load Control Points")){
		LoadControlPoints(GetName() + "_control_points.json");
	}

	ImGui::End();
}

void DiagonalSlashRightToLeft::SaveControlPoints(const std::string& filepath) const{
	nlohmann::json j;
	for (const auto& point : controlPoints_){
		j["controlPoints"].push_back({{"x", point.x}, {"y", point.y}, {"z", point.z}});
	}

	std::ofstream file(filepath);
	if (file.is_open()){
		file << j.dump(4);
		file.close();
	}
}

void DiagonalSlashRightToLeft::LoadControlPoints(const std::string& filepath){
	nlohmann::json j;
	std::ifstream file(filepath);
	if (file.is_open()){
		file >> j;
		file.close();

		controlPoints_.clear();
		for (const auto& point : j["controlPoints"]){
			Vector3 v;
			v.x = point["x"].get<float>();
			v.y = point["y"].get<float>();
			v.z = point["z"].get<float>();
			controlPoints_.emplace_back(v);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//						collision
//////////////////////////////////////////////////////////////////////////
void DiagonalSlashRightToLeft::OnCollisionEnter([[maybe_unused]] Collider* other){
	//* 衝突相手がtargetType_に含まれていなければreturn
	if ((other->GetType() & Collider::GetTargetType()) != ColliderType::Type_None){

		IPlayerAttack::OnCollisionEnter(other);

	}
}

void DiagonalSlashRightToLeft::OnCollisionStay([[maybe_unused]] Collider* other){}

void DiagonalSlashRightToLeft::OnCollisionExit([[maybe_unused]] Collider* other){}
