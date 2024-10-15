#pragma once

#include "objects/Model.h"
#include "graphics/ViewProjection.h"

class Character{
public:
	Character() = default;
	virtual ~Character() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(Model* model);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();
	
	void SetViewProjection(const ViewProjection* viewProjection);

protected:
	//描画モデル
	Model* model_ = nullptr;
};
