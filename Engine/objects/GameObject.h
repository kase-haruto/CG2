#pragma once
#include"DirectXCommon.h"
#include"ViewProjection.h"
#include"PipelineStateManager.h"

class GameObject{
public:
	GameObject(){}
	virtual ~GameObject(){}

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	/// <param name="viewProjection"></param>
	/// <param name="psManager"></param>
	virtual void Initialize(ViewProjection* viewProjection) = 0;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;
};

