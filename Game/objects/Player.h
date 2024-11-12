#pragma once

#include "objects/Character.h"

#include "objects/Bullet.h"
#include "Line.h"

#include "objects/Sprite.h"

#include <list>
#include <memory>

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
	void UpdateUI();

	void BeamUpdate();

	//レティクルの更新
	void ReticleUpdate();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;
	void DrawUi();

	void OnCollision(Collider* other)override;


	/////////////////////////////////////////////////////////////////////////////////////////
	//			アクセッサ
	/////////////////////////////////////////////////////////////////////////////////////////

	void SetParent(const Transform* parent){ parentTransform_ = parent; }

	const Vector3 GetCenterPos()const override;


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

	//前方ベクトルの取得
	const Vector3 GetForwardVector()const;

	const Vector3 GetRightVector() const;

	const Segment& GetLine()const{ return line_; }

private:
	void Shoot();

private:
	bool isShoot_ = false;

	//親のトラスフォーム
	const Transform* parentTransform_ = nullptr;

	Matrix4x4 parentWorldMat_ {};

	Vector3 reticlePos_;

	std::list<std::unique_ptr<Bullet>>bullets_;

	//ビーム衝突用
	Segment line_;

	//ビーム描画用
	std::unique_ptr<Model>beam_ = nullptr;

	float shootTimer_ = 5.0f;            // ビーム発射可能な最大時間
	const float maxShootTime_ = 5.0f;    // 最大時間（5秒）
	const float recoveryRate_ = 0.1f;    // shootTimerの回復速度
	bool isFullyRecovered_ = true;		 // タイマーが完全に回復したかどうかのフラグ

	std::unique_ptr<Sprite>hpSprite_ = nullptr;
	std::unique_ptr<Sprite>reticleSprite_ = nullptr;
};
