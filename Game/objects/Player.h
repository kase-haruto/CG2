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

	void SetCtrlPoints(const std::vector<Vector3>& controlPoint){ ctrlPoints_ = controlPoint; }
	void SetParent(const Transform* parent){ parentTransform_ = parent; }

	void SetIsRail(const bool isRail){ isRail_ = isRail; }

private:
	std::vector<Vector3>ctrlPoints_;
	float t_;
	//親のトラスフォーム
	const Transform* parentTransform_ = nullptr;
	Matrix4x4 parentWorldMat_ {};

	bool isRail_ = false;
};
