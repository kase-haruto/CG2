#pragma once

#include "Model.h"

class Bullet{
public:
	Bullet() = default;
	~Bullet();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model);
	void Initialize(Model* model,const Vector3& pos,const Vector3& vel);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// viewProjectionをセット
	/// </summary>
	/// <param name="viewProjection"></param>
	void SetViewProjection(const ViewProjection* viewProjection);

	/// <summary>
	/// トランスフォームを取得
	/// </summary>
	/// <returns></returns>
	const Transform& GetTransform()const{ return model_->transform; }

	/// <summary>
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

	/// <summary>
	/// 速度を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetVelocity()const{ return velocity_; }

	/// <summary>
	/// 生存フラグを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsActive()const{ return isActive_; }


private:
	//描画用モデル
	Model* model_ = nullptr;

	//速度
	Vector3 velocity_;

	//生存フラグ
	bool isActive_ = true;

	//寿命(5秒)
	static const int32_t kLifeTime =300;

	//デスタイマー
	int32_t deathTimer_ = kLifeTime;
};

