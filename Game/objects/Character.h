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
	
	/// <summary>
	/// viewProjectionをセット
	/// </summary>
	/// <param name="viewProjection"></param>
	void SetViewProjection(const ViewProjection* viewProjection);

	/// <summary>
	/// トランスフォームを取得
	/// </summary>
	/// <returns></returns>
	const Transform& GetTransform()const{ return model_->transform; }

protected:
	//描画モデル
	Model* model_ = nullptr;
};
