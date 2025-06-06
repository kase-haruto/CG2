#include "Viewport.h"
/* ========================================================================
/* include space
/* ===================================================================== */

// engine
#include <Engine/Graphics/Camera/Base/BaseCamera.h>

#include <externals/imgui/ImGuizmo.h>

Viewport::Viewport(ViewportType type, const std::string& windowName) :type_(type), windowName_(windowName){}

void Viewport::Render(const ImTextureID& tex){
	textureID_ = tex; // テクスチャIDを設定
	ImGui::Begin(windowName_.c_str(), nullptr,
				 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	ImVec2 contentSize = ImGui::GetContentRegionAvail();
	size_ = Vector2(contentSize.x, contentSize.y);

	// カメラのアスペクト比を更新
	if (size_.y > 0.0f){
		camera_->SetAspectRatio(size_.x / size_.y);
	}

	// 画像を描画
	if (textureID_){
		ImVec2 imagePos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(imagePos);
		ImGui::Image(textureID_, contentSize);

		if (type_ != ViewportType::VIEWPORT_DEBUG){
			// ImGuizmo範囲設定（Debug専用にしてもOK）
			ImGuizmo::SetRect(imagePos.x, imagePos.y, size_.x, size_.y);
			ImGuizmo::SetDrawlist();
		}
	} else{
		ImGui::Text("Viewport texture not set.");
	}

	isHovered_ = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	isClicked_ = ImGui::IsWindowFocused() && ImGui::IsMouseDown(0);

	ImGui::End();
}

bool Viewport::IsHovered() const{ return isHovered_; }
bool Viewport::IsClicked() const{ return isClicked_; }
Vector2 Viewport::GetSize() const{ return size_; }
ViewportType Viewport::GetType() const{ return type_; }
void Viewport::SetCamera(BaseCamera* camera){ camera_ = camera; }

