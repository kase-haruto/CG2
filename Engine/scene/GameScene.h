#pragma once

#include"IScene.h"

#include"ViewProjection.h"
#include"Rail/RailEditor.h"
#include "objects/Player.h"

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
	///=========================
	/// カメラ関連
	///=========================
	std::unique_ptr<ViewProjection> viewProjection_ = nullptr;


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
	std::unique_ptr<Player> player_ = nullptr;
	
};
