#pragma once 

/* engine */
#include"collision/CollisionTypeIdDef.h"

/* lib */
#include "Vector3.h"

#include<cstdint>

class Collider{
public:
	//*コンストラクタ・デストラクタ
	Collider() = default;
	virtual ~Collider() = default;

	virtual void OnCollision(Collider* other) = 0;
	virtual const Vector3 GetCenterPos()const = 0;

	/////////////////////////////////////////////////////////////////////////////////////////
	//				アクセッサ
	/////////////////////////////////////////////////////////////////////////////////////////
	float GetRadius()const{ return radius_; };
	void SetRadius(const float radius){ radius_ = radius; }

	//識別IDの取得
	uint32_t GetTypeID()const{ return typeID_; }
	void SetTypeID(uint32_t typeID){ typeID_ = typeID; }

private:
	//衝突識別番号
	uint32_t typeID_ = 0u;

	//衝突判定
	float radius_ = 1.0f;
};
