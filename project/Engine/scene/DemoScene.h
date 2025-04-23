#pragma once

#include "IScene.h"

#include "Engine/graphics/FogEffect.h"
#include "Engine/physics/light/DirectionalLight.h"
#include "Engine/physics/light/PointLight.h"

#include <memory>

//===================================================================*/
// DemoSceneクラス
// ISceneを継承したデモ用のシーン。
// 何も機能を持たない空のシーン実装例。
//===================================================================*/
class DemoScene final : public IScene{
public:
	//===================================================================*/
	//                   コンストラクタ・デストラクタ
	//===================================================================*/
	DemoScene() = default;
	DemoScene(DxCore* dxCore);
	~DemoScene() override = default;

	void Initialize() override;      // 初期化処理
	void Update() override;          // 更新処理 
	void Draw() override;            // 描画処理
	void CleanUp() override;        // 終了処理
	void Draw3dObject() override;	// モデル描画前処理

private:
	//===================================================================*/
	//                   private fields
	//===================================================================*/

	//* グラフィック関連 *//
	std::unique_ptr<DirectionalLight>directionalLight_ = nullptr;
	std::unique_ptr<PointLight> pointLight_ = nullptr;
	std::unique_ptr<FogEffect>fog_ = nullptr;
};
