#include "Viewport.h"
/* ========================================================================
/* include space
/* ===================================================================== */

// engine
#include <Engine/Graphics/Camera/Base/BaseCamera.h>

#include <externals/imgui/ImGuizmo.h>

Viewport::Viewport(ViewportType type, const std::string& windowName) :type_(type), windowName_(windowName){}

void Viewport::Update(){}

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

		ImVec2 viewportPos = imagePos;
		ImVec2 viewportSize = ImVec2(size_.x, size_.y);

		ImGui::BeginGroup();
		for (auto* tool : tools_){
			auto* base = dynamic_cast< BaseOnViewportTool* >(tool);
			if (!base) continue;

			ImVec2 viewSize = ImVec2(size_.x, size_.y);

			ImVec2 pos = base->CalcScreenPosition(imagePos, viewSize);
			tool->RenderOverlay(pos); // ← 基準位置を引数で渡す
		}

		ImGui::EndGroup();
	}

	isHovered_ = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	isClicked_ = ImGui::IsWindowFocused() && ImGui::IsMouseDown(0);

	ImGui::End();
}

ImVec2 Viewport::CalcToolPosition(IOnViewportTool* tool, const ImVec2& viewportPos, const ImVec2& viewportSize){
	ImVec2 basePos;

	OverlayAlign align = OverlayAlign::TopLeft;
	if (auto* base = dynamic_cast< BaseOnViewportTool* >(tool)){
		align = base->GetOverlayAlign();
	}

	switch (align){
		case OverlayAlign::TopLeft:
			basePos = viewportPos;
			break;
		case OverlayAlign::TopRight:
			basePos = ImVec2(viewportPos.x + viewportSize.x, viewportPos.y);
			break;
		case OverlayAlign::BottomLeft:
			basePos = ImVec2(viewportPos.x, viewportPos.y + viewportSize.y);
			break;
		case OverlayAlign::BottomRight:
			basePos = ImVec2(viewportPos.x + viewportSize.x, viewportPos.y + viewportSize.y);
			break;
		case OverlayAlign::CenterTop:
			basePos = ImVec2(viewportPos.x + viewportSize.x * 0.5f, viewportPos.y);
			break;
	}

	// オフセット加算
	return ImVec2(basePos.x + tool->GetOverlayOffset().x,
				  basePos.y + tool->GetOverlayOffset().y);
}

void Viewport::AddTool(IOnViewportTool* tool){ tools_.push_back(tool); }

bool Viewport::IsHovered() const{ return isHovered_; }
bool Viewport::IsClicked() const{ return isClicked_; }
Vector2 Viewport::GetSize() const{ return size_; }
Vector2 Viewport::GetPosition() const{
	// ビューポートの位置を取得
	return viewOrigin_;
}
ViewportType Viewport::GetType() const{ return type_; }
void Viewport::SetCamera(BaseCamera* camera){ camera_ = camera; }

