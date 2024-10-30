#pragma once 
#include "objects/Character.h"


class Enemy 
    :public Character{
public: 
    /// <summary>
    /// コンストラクタ/
    /// デストラクタ
    /// </summary>
    Enemy(); 
    ~Enemy() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	void Initialize(Model* model)override;

	void Initialize(Model* model,const Vector3& pos);

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	void OnCollision(Collider* other);

	/////////////////////////////////////////////////////////////////////////////////////////
	//			アクセッサ
	/////////////////////////////////////////////////////////////////////////////////////////

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

	const Vector3 GetCenterPos()const;

	//座標を入れる
	void SetPos(const Vector3& pos){ model_->transform.translate = pos; }

	//生存フラグの取得
	bool GetIsAlive()const{ return isAlive_; }

private: 
	void Move();

private:
	Vector3 velocity_ {};

	//生存フラグ
	bool isAlive_ = true;
}; 
