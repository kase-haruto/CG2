#include "Viewport.h"
/* ========================================================================
/* include space
/* ===================================================================== */

// engine
#include <Engine/Graphics/Camera/Base/BaseCamera.h>

#include <externals/imgui/ImGuizmo.h>

Viewport::Viewport(ViewportType type, const std::string& windowName) :type_(type), windowName_(windowName){}

void Viewport::Update(){
	for (auto& tool:tools_){
		tool->Update();
	}
}

void Viewport::Render(const ImTextureID& tex){
	textureID_ = tex;
	ImGui::Begin(windowName_.c_str(), nullptr,
				 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	ImVec2 contentSize = ImGui::GetContentRegionAvail();
	size_ = Vector2(contentSize.x, contentSize.y);
	// 画像描画
	ImVec2 imagePos = ImGui::GetCursorScreenPos();
	viewOrigin_ = Vector2(imagePos.x, imagePos.y);
	if (size_.y > 0.0f){
		camera_->SetAspectRatio(size_.x / size_.y);
	}

	ImGui::SetCursorScreenPos(imagePos);
	ImGui::Image(textureID_, contentSize);

	if (type_ == ViewportType::VIEWPORT_DEBUG){
		ImGuizmo::SetRect(imagePos.x, imagePos.y, size_.x, size_.y);
		ImGuizmo::SetDrawlist();

		// オーバーレイツールバーの描画位置を設定（画像の左上にする場合）
		ImGui::SetCursorScreenPos(ImVec2(imagePos.x + 10, imagePos.y + 20));
		ImGui::BeginGroup();
		for (auto* tool : tools_){
			tool->RenderToolbar();	
		}
		ImGui::EndGroup();
	}

	isHovered_ = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	isClicked_ = ImGui::IsWindowFocused() && ImGui::IsMouseDown(0);

	ImGui::End();
}

void Viewport::AddTool(IOnViewportTool* tool){ tools_.push_back(tool); }

bool Viewport::IsHovered() const{ return isHovered_; }
bool Viewport::IsClicked() const{ return isClicked_; }
Vector2 Viewport::GetSize() const{ return size_; }
Vector2 Viewport::GetPosition() const {
	// ビューポートの位置を取得
	return viewOrigin_;
}
ViewportType Viewport::GetType() const{ return type_; }
void Viewport::SetCamera(BaseCamera* camera){ camera_ = camera; }

