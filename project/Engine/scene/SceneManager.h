#pragma once

#include <memory>
#include "IScene.h"
#include "Engine/core/EngineUI.h"
#include "Engine/core/DirectX/DxCore.h"

//===================================================================*/
// SceneManagerクラス
// シーンの生成・破棄、シーン切り替えなどを管理する
//===================================================================*/
class SceneManager{
public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	SceneManager() = default;
	SceneManager(DxCore* dxCore){ pDxCore_ = dxCore; }
	~SceneManager();

	//===================================================================*/
	//                   シーン管理
	//===================================================================*/
	// 初期シーンの設定と初期化
	void Initialize();

	// シーン更新処理
	// 予約された次シーンがあればここで切り替える
	void Update();

	// シーン描画処理
	void Draw();

	// 次シーンを予約
	void SetNextScene(std::unique_ptr<IScene> scene){ nextScene_ = std::move(scene); }

	// UIをセット
	void SetEngineUI(EngineUI* ui){ pEngineUI_ = ui; }

private:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	// 次シーンへの切り替え処理
	void SwitchToNextScene();

	//===================================================================*/
	//                   private fields
	//===================================================================*/
	std::unique_ptr<IScene> nextScene_ = nullptr;    // 次のシーン
	std::unique_ptr<IScene> currentScene_ = nullptr; // 現在のシーン

	EngineUI* pEngineUI_ = nullptr;  // ui
	DxCore* pDxCore_ = nullptr;      // dxcore
};
