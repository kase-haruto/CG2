#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/Graphics/Camera/Viewport/ViewportDetail.h>
#include <Engine/Foundation/Math/Vector2.h>

// engine
#include <Engine/Application/UI/EngineUI/IOnViewportTool.h>

// c++
#include <string>

// externals
#include <externals/imgui/imgui.h>

// forward declaration
struct Vector2;
class BaseCamera;

class Viewport{
public:
	//===================================================================*/
	//                    methods
	//===================================================================*/
	Viewport(ViewportType type, const std::string& windowName);

	void Update();								//< ビューポートの更新処理
	void Render(const ImTextureID& tex);		//< ImGui上への描画処理
	void AddTool(IOnViewportTool* tool);		//< ビューポートツールの追加

	//--------- accessor -----------------------------------------------------
	bool IsHovered() const;
	bool IsClicked() const;
	Vector2 GetSize() const;
	ViewportType GetType() const;
	void SetCamera(BaseCamera* camera);
private:
	std::vector<IOnViewportTool*> tools_;
	ViewportType type_ = ViewportType::VIEWPORT_NONE;	//< ビューポートの種類
	std::string windowName_;							//< ビューポートのウィンドウ名

	ImTextureID textureID_ = nullptr;

	BaseCamera* camera_ = nullptr;						//< ビューポートに関連付けられたカメラ
	Vector2 size_ {};
	bool isHovered_ = false;
	bool isClicked_ = false;
};

