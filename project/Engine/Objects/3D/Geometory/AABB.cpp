#include "AABB.h"


/* external */
#include "externals/imgui/imgui.h"
/* c++ */
#include <cmath>


void AABB::Initialize(const Vector3& Min, const Vector3& Max) {
	min_ = Min;
	max_ = Max;
}

void AABB::Update() {
	//minとmaxが入れ替わらないようにする
	min_.x = (std::min)(min_.x, max_.x);
	max_.x = (std::max)(min_.x, max_.x);

	min_.y = (std::min)(min_.y, max_.y);
	max_.y = (std::max)(min_.y, max_.y);

	min_.z = (std::min)(min_.z, max_.z);
	max_.z = (std::max)(min_.z, max_.z);
}



void AABB::UpdateUI(std::string lavel) {
	ImGui::Begin(lavel.c_str());
	ImGui::DragFloat3("min", &min_.x, 0.01f);
	ImGui::DragFloat3("max", &max_.x, 0.01f);
	ImGui::End();
}

Vector3 AABB::GetMin()const { return min_; }
Vector3 AABB::GetMax()const { return max_; }

