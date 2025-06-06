#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */

#include <Engine/Application/UI/EngineUI/IOnViewportTool.h>

#include <externals/imgui/imgui.h>
#include <externals/imgui/ImGuizmo.h>

class WorldTransform;
class BaseCamera;
struct Matrix4x4;

class Manipulator 
	: public IOnViewportTool{
public:
	void Update() override;
	void RenderOverlay() override;
	void RenderToolbar() override;

	void SetTarget(WorldTransform* target);
	void SetCamera(BaseCamera* camera);
	void SetViewRect(const ImVec2& origin, const ImVec2& size);

private:
	void RowToColumnArray(const Matrix4x4& m, float out[16]);
	Matrix4x4 ColumnArrayToRow(const float in_[16]);

private:
	ImGuizmo::OPERATION operation_ = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE mode_ = ImGuizmo::WORLD;

	WorldTransform* target_ = nullptr;
	BaseCamera* camera_ = nullptr;

	ImVec2 viewOrigin_ = {0, 0};
	ImVec2 viewSize_ = {0, 0};
};