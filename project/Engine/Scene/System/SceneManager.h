#pragma once
/* ========================================================================
/*  include space
/* ===================================================================== */
// engine
#include <Engine/scene/Base/BaseScene.h>
#include <Engine/Application/UI/EngineUI/Core/EngineUICore.h>
#include <Engine/Graphics/Device/DxCore.h>

// c++
#include <memory>
#include <array>

class IRenderTarget;

class SceneManager{
public:
	SceneManager() = default;
	SceneManager(DxCore* dxCore);
	~SceneManager();

	// 初期化(シーン生成など)
	void Initialize();

	// シーン更新
	void Update();

	// シーン描画
	void Draw();
	void DrawForRenderTarget(IRenderTarget* target);
	// UIをセット
	void SetEngineUI(EngineUICore* ui){ pEngineUI_ = ui; }
	// 切り替えリクエスト用メソッド (UI などから呼び出す)
	void RequestSceneChange(SceneType nextScene);
	void SetCurrentScene(std::unique_ptr<IScene> newScene);

private:
	// シーンインスタンスの配列
	std::array<std::unique_ptr<IScene>, static_cast< int >(SceneType::count)> scenes_;

	// 現在シーン・次シーン
	int currentSceneNo_ {static_cast< int >(SceneType::PLAY)};
	int nextSceneNo_ {static_cast< int >(SceneType::PLAY)};

	// UIパネルなど
	EngineUICore* pEngineUI_ = nullptr;
	DxCore* pDxCore_ = nullptr;

public:
	bool gameResult_ = false;
};
