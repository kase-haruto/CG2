#pragma once

/* ========================================================================
/*	include space
/* ===================================================================== */

// engine
#include <Engine/Application/UI/EngineUI/IEngineUI.h>
#include <Engine/Objects/3D/Actor/SceneObject.h>

// c++
#include <vector>
#include <memory>
#include <functional>
#include <d3d12.h>

class EngineUIRenderer {
public:
	//===================================================================*/
	//					public function
	//===================================================================*/
	EngineUIRenderer() = default;       // コンストラクタ
	~EngineUIRenderer() = default;      // デストラクタ

	void Initialize();					 // 初期化
	void Render();						 // レンダリング
	void Finalize();					 // 終了処理

	void AddPanel(std::unique_ptr<IEngineUI> panel); // パネル追加
	void RemovePanel(const std::string& panelName); // パネル削除

	void SetMainViewportTexture(UINT64 textureID);  // メインビューポート用のテクスチャを設定
	void SetDebugViewportTexture(UINT64 textureID); // デバッグビューポート用のテクスチャを設定

	//パネルの取得
	template<class Panel>
	Panel* GetPanel() {
		for (auto& panel : panels_) {
			if (auto p = dynamic_cast<Panel*>(panel.get())) {
				return p;
			}
		}
		return nullptr;
	}

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
	std::vector<std::unique_ptr<IEngineUI>> panels_;

	UINT64 mainViewportTextureID_ = 0;
	UINT64 debugViewportTextureID_ = 0;
};