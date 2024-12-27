#pragma once

#include "Engine/objects/Collider/BoxCollider.h"

class IPlayerAttack:
	public BoxCollider{
public:
	//===================================================================*/
	//					public member functions
	//===================================================================*/
	IPlayerAttack(const std::string& attackName);
	virtual ~IPlayerAttack();

	virtual void Initialize() = 0;	//< 初期化
	virtual void Execution() = 0;	//< 実行
	virtual void Update();			//< 更新
	virtual void Draw();			//< 描画
	virtual void ShowGui();			//< gui表示

	/* collision ===================================*/
	virtual void OnCollisionEnter([[maybe_unused]]Collider* other)override{}
	virtual void OnCollisionStay([[maybe_unused]] Collider* other)override{}
	virtual void OnCollisionExit([[maybe_unused]] Collider* other)override{}

protected:
	//===================================================================*/
	//					protected member variables
	//===================================================================*/
	Vector3 center_;			//< 衝突判定用の中心座標
	Vector3 offset_;			//< 衝突判定用のオフセット
	Vector3 rotate_;			//< 衝突判定用の回転角度

	bool isAttacking_ = false;	//< 攻撃中フラグ

public:
	//===================================================================*/
	//					getter/setter
	//===================================================================*/
	void SetCenter(const Vector3& center){ center_ = center; }
};

