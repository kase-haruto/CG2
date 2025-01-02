#include "WeakDiagonalSlash.h"

#include "Engine/core/System.h"
#include "lib/myFunc/MathFunc.h"
#include "Engine/core/Json/JsonCoordinator.h"

WeakDiagonalSlash::WeakDiagonalSlash(const std::string& attackName)
	: IPlayerAttack(attackName),
	animationTime_(0.0f),
	animationSpeed_(1.5f) // 必要に応じて調整
{
	SetupDefaultControlPoints();
}

//////////////////////////////////////////////////////////////////////////
//						main functions
//////////////////////////////////////////////////////////////////////////
void WeakDiagonalSlash::Initialize(){
	isAttacking_ = true;
	animationTime_ = 0.0f;
}

void WeakDiagonalSlash::Execution(){
	Initialize();
}

void WeakDiagonalSlash::Update(){
	if (!isAttacking_) return;

	animationTime_ += animationSpeed_ * System::GetDeltaTime();
	if (animationTime_ > 1.0f){
		animationTime_ = 1.0f;
		isAttacking_ = false;
	}

	// Catmull-Rom 補間を使用して現在の位置を計算
	currentPosition_ = CatmullRomPosition(controlPoints_, animationTime_);

	// 武器の位置を基準に攻撃オブジェクトの位置を設定
	//Vector3 weaponPosition = weapon_->GetCenterPos(); // SetCenter で設定された武器の位置を取得
	weapon_->SetPosition(currentPosition_);

	shape_.center = center_ + offset_;
}

void WeakDiagonalSlash::Draw(){
	if (isAttacking_){
		BoxCollider::Draw();
	}
}

//////////////////////////////////////////////////////////////////////////
//						helper functions
//////////////////////////////////////////////////////////////////////////
void WeakDiagonalSlash::SetupDefaultControlPoints(){
	// デフォルトの制御点を設定
	controlPoints_.emplace_back(Vector3(-0.5f, 0.0f, 0.0f)); // p0
	controlPoints_.emplace_back(Vector3(0.0f, 0.5f, 0.0f));  // p1
	controlPoints_.emplace_back(Vector3(0.5f, 0.0f, 0.0f));  // p2
	controlPoints_.emplace_back(Vector3(0.0f, -0.5f, 0.0f)); // p3
}

std::unique_ptr<IPlayerAttack> WeakDiagonalSlash::Clone() const{
	return std::make_unique<WeakDiagonalSlash>(*this);
}


//////////////////////////////////////////////////////////////////////////
//						imgui/ui
//////////////////////////////////////////////////////////////////////////
void WeakDiagonalSlash::ShowGui(){
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

void WeakDiagonalSlash::SaveControlPoints(const std::string& filepath) const{
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

void WeakDiagonalSlash::LoadControlPoints(const std::string& filepath){
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
void WeakDiagonalSlash::OnCollisionEnter([[maybe_unused]] Collider* other){}

void WeakDiagonalSlash::OnCollisionStay([[maybe_unused]] Collider* other){}

void WeakDiagonalSlash::OnCollisionExit([[maybe_unused]] Collider* other){}
