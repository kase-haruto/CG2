#pragma once

#include"IScene.h"

#include"ViewProjection.h"
#include"Rail/RailEditor.h"
#include "Rail/Rail.h"
#include "objects/Player.h"
#include "Rail/RailCamera.h"
#include "objects/EnemyManager.h"
#include "objects/Sprite.h"
#include "Editor/UiEditor.h"

///デバッグ関連///
#ifdef _DEBUG
#include"ImGuiManager.h"
#include"imgui.h"
#endif // _DEBUG


#include<memory>

class GameScene final
	:public IScene{
public:
	GameScene();
	~GameScene()override = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw()override;

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize()override;

private:
	//スコアのスプライトの更新
	void UpdateScore();
	//スコアのスプライトの表示
	void DrawScore();

private:
	///=========================
	/// カメラ関連
	///=========================
	std::unique_ptr<ViewProjection> viewProjection_ = nullptr;
	std::unique_ptr<RailCamera> railCamera_ = nullptr;

	///=========================
	/// Editor関連
	///======================
	std::unique_ptr<RailEditor> railEditor_ = nullptr;

	///=========================
	/// 描画用モデル
	///=========================
	std::unique_ptr<Model> playerModel_ = nullptr;

	///=========================
	/// 3dObject
	///=========================
	//*skydome
	std::unique_ptr<Model>skydome_ = nullptr;

	/// ingameObject
	std::unique_ptr<Player> player_ = nullptr;
	std::unique_ptr<EnemyManager> enemyManager_ = nullptr;

	//Rail
	Rail rail_;

	bool isRail_ = false;

	//敵のスポーンタイマー
	uint32_t spawnTime_ = 0;
	const uint32_t spawnTimeLimit_ = 300;

public:
	uint32_t score_;
	static uint32_t newScore_;

	std::unique_ptr<UIEditor>editor_ = nullptr;

	//表示用スコア
	std::vector<std::shared_ptr<Sprite>> scoreSprites_;
	std::unique_ptr<Sprite> test_;
};
