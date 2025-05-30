#pragma once

/* ========================================================================
/*	include space
/* ===================================================================== */

// engine
#include <Engine/Application/UI/EngineUI/IEngineUI.h>
#include <Engine/Application/UI/Panels/Controller/PanelController.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>

// c++
#include <vector>
#include <memory>
#include <functional>
#include <d3d12.h>

class EngineUICore {
public:
	//===================================================================*/
	//					public function
	//===================================================================*/
	EngineUICore() = default;       // コンストラクタ
	~EngineUICore() = default;      // デストラクタ

	void Initialize();					 // 初期化
	void Render();						 // レンダリング

	void AddPanel(std::unique_ptr<IEngineUI> panel); // パネル追加

	void SetMainViewportTexture(UINT64 textureID);  // メインビューポート用のテクスチャを設定
	void SetDebugViewportTexture(UINT64 textureID); // デバッグビューポート用のテクスチャを設定

	//パネルの取得
	template<class Panel>
	Panel* GetPanel();
private:
	//===================================================================*/
	//					private function
	//===================================================================*/
	void RenderMainViewport();  // メインビューポートの描画
	void RenderDebugViewPort(); // デフォルトビューポートの描画
	void RenderMenue();         // メニューの描画

private:
	//===================================================================*/
	//					private variable
	//===================================================================*/
	std::unique_ptr<PanelController> panelController_ = nullptr;
	UINT64 mainViewportTextureID_ = 0;
	UINT64 debugViewportTextureID_ = 0;
};

template<class Panel>
Panel* EngineUICore::GetPanel() {
	Panel temp;
	const std::string& name = temp.GetPanelName();
	IEngineUI* base = panelController_->GetPanel(name);
	return dynamic_cast<Panel*>(base);
}
