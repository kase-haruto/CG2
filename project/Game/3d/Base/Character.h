#pragma once
#include "../BaseGameObject.h"

#include <cstdint>

class Character :
    public BaseGameObject{
public:
	//===================================================================*/
	//                   public methods
	//===================================================================*/
	Character() = default;
	Character(const std::string& modelName);
	virtual ~Character() override = default;

	virtual void Initialize()override;
	virtual void Update()override;


	/* ui =========================================*/
	virtual void ShowGui()override;

protected:
	//===================================================================*/
	//                   private methods
	//===================================================================*/
	float moveSpeed_ = 0.0f;	//* 移動速度
	Vector3 velocity_ = {};		//* 移動ベクトル
	Vector3 acceleration_ = {};	//* 加速度
	int32_t life_ = 1;			//* 体力	(0で死亡)
	bool isAlive_;				//* 生存フラグ
	bool onGround_;				//* 地面に接地しているか

	//===================================================================*/
	//                   getter/setter
	//===================================================================*/
public:
	void velocity(const Vector3& velocity){ velocity_ = velocity; }

};

