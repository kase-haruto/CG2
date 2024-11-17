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
#include "objects/ModelBuilder.h"

#include "Game/objects/Boss.h"

///デバッグ関連///
#ifdef _DEBUG
#include"ImGuiManager.h"
#include"imgui.h"
#endif // _DEBUG


#include<memory>

enum GameState{
	START,	//*play前の待機画面
	PLAY,	//* play画面
	END		//* 終わり
};

class GameScene final
	:public IScene{
public:
	GameScene();
	~GameScene()override;

	/////////////////////////////////////////////////////////////////////////////////////////
	//		public
	/////////////////////////////////////////////////////////////////////////////////////////


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
	/////////////////////////////////////////////////////////////////////////////////////////
	//		private
	/////////////////////////////////////////////////////////////////////////////////////////
	//スコアのスプライトの更新
	void UpdateScore();
	//スコアのスプライトの表示
	void DrawScore();

private:
	void StartUpdate();
	void PlayUpdate();
	void ResultUpdate();

	void StartDraw();
	void PlayDraw();
	void ResultDraw();

	void Reset();


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
	std::unique_ptr<Model> bossModel_ = nullptr;
	std::unique_ptr<Model> coaster_ = nullptr;
	std::unique_ptr<ModelBuilder> modelBuilder_ = nullptr;
	///=========================
	/// 3dObject
	///=========================
	//*skydome
	std::unique_ptr<Model>skydome_ = nullptr;
	std::unique_ptr<Model>ground_ = nullptr;

	/// ingameObject
	std::unique_ptr<Boss> boss_ = nullptr;
	std::unique_ptr<Player> player_ = nullptr;
	std::unique_ptr<EnemyManager> enemyManager_ = nullptr;

	//Rail
	Rail rail_;

	bool isRail_ = true;

	//敵のスポーンタイマー
	uint32_t spawnTime_ = 0;
	const uint32_t spawnTimeLimit_ = 60;

public:
	uint32_t score_;
	static uint32_t newScore_;

	std::unique_ptr<UIEditor>editor_ = nullptr;

	//表示用スコア
	std::vector<std::shared_ptr<Sprite>> scoreSprites_;
	std::vector<std::shared_ptr<Sprite>> resultScoreSprites_;
	std::unique_ptr<Sprite> test_;
	std::unique_ptr<Sprite> scoreFrame_ = nullptr;

	std::unique_ptr<Sprite> space_ = nullptr;

	//タイトル
	std::unique_ptr<Sprite> title_ = nullptr;

	//gamecen の状態
	GameState gameState_;
};
