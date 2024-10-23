#pragma once

#include "objects/Character.h"

#include "objects/Bullet.h"

#include <list>

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

	/// ワールド座標の取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition()const{
		Vector3 wPos;
		// ワールド行列の平行移動成分を取得
		wPos.x = model_->worldMatrix.m[3][0];
		wPos.y = model_->worldMatrix.m[3][1];
		wPos.z = model_->worldMatrix.m[3][2];

		return wPos; // 値をコピーして返す
	}

private:
	void Shoot();

private:
	std::vector<Vector3>ctrlPoints_;
	float t_;
	//親のトラスフォーム
	const Transform* parentTransform_ = nullptr;

	Matrix4x4 parentWorldMat_ {};

	bool isRail_ = false;

	std::list<std::unique_ptr<Bullet>>bullets_;

};
