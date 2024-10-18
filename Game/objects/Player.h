#pragma once

#include "objects/Character.h"

class Player
	:public Character{
public:
	Player() = default;
	~Player()override = default;


	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	void Initialize(Model* model)override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

private:

};
