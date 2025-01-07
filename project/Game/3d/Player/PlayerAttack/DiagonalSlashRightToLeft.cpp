#include "DiagonalSlashRightToLeft.h"
#include "Engine/core/System.h"
#include "lib/myFunc/MathFunc.h"
#include "Engine/core/Json/JsonCoordinator.h"
#include "../Player.h"

#include "Engine/core/Audio/Audio.h"
#include "Engine/core/Input.h"
#include "Engine/graphics/camera/CameraManager.h"

#include "lib/myFunc/MyFunc.h"
#include "Engine/graphics/GraphicsGroup.h"
#include <externals/imgui/imgui.h>

DiagonalSlashRightToLeft::DiagonalSlashRightToLeft(const std::string& attackName)
	: IPlayerAttack(attackName),
	animationTime_(0.0f),
	animationSpeed_(2.0f) // 必要に応じて調整
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
	auto graphics = GraphicsGroup::GetInstance();
	swordTrail_.Initialize(
		graphics->GetDevice(),
		graphics->GetCommandList(),
		graphics->GetRootSignature(Object3D),      // 例: Object3D 用ルートシグネチャ
		graphics->GetPipelineState(Object3D)       // 例: Object3D 用PSO
	);

	// フェードアウトの速度や最小アルファなどを調整 (必要に応じて)
	swordTrail_.SetFadeSpeed(2.0f);  // 1秒あたりアルファが2.0 減衰
	swordTrail_.SetMinAlpha(0.05f);  // 0.05以下で削除

	Audio::Play("attack.mp3", false);

	// 初期回転を設定し、初期回転を保存
	initialRotate_ = {0.0f, 0.48f, -0.59f};
	weapon_->SetRotate(initialRotate_);
}

void DiagonalSlashRightToLeft::Execution(){
	Initialize();
}

void DiagonalSlashRightToLeft::Update(){
	if (!isAttacking_) return;

	// アニメーション時間を更新
	animationTime_ += animationSpeed_ * System::GetDeltaTime();
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
void DiagonalSlashRightToLeft::OnCollisionEnter([[maybe_unused]] Collider* other){}

void DiagonalSlashRightToLeft::OnCollisionStay([[maybe_unused]] Collider* other){}

void DiagonalSlashRightToLeft::OnCollisionExit([[maybe_unused]] Collider* other){}
