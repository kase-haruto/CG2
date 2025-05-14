#include "Collider.h"

#include <Engine/Collision/CollisionManager.h>

#include <externals/imgui/imgui.h>


Collider::Collider(bool isEnuble) {
	isCollisionEnabled_ = isEnuble;
	if (isCollisionEnabled_) {
		CollisionManager::GetInstance()->Register(this);
	}
}

void Collider::ShowGui() {
    bool enabled = isCollisionEnabled_;
    if (ImGui::Checkbox("Enable Collision", &enabled)) {
        SetCollisionEnabled(enabled); // 状態が変わったときだけ反映
    }

    if (!isCollisionEnabled_) return;

    ImGui::Checkbox("Draw Collider", &isDraw_);
    ImGui::ColorEdit4("Collider Color", &color_.x);
}


void Collider::SetCollisionEnabled(bool enable) {
    if (isCollisionEnabled_ == enable) return; // 状態が変わらないなら何もしない

    isCollisionEnabled_ = enable;

    if (enable) {
        CollisionManager::GetInstance()->Register(this);
    } else {
        CollisionManager::GetInstance()->Unregister(this);
    }
}