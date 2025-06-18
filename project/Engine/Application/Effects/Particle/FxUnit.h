#pragma once

#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Foundation/Math/Vector4.h>

#include <string>

struct FxUnit{
	Vector3 position;		//< 座標
	Vector3 velocity;		//< 速度
	float lifetime = 1.0f;	//< 寿命
	float age;				//< 経過時間
	Vector4 color;			//< 色
	float size;				//< サイズ
	bool alive = true;		//< 生存フラグ
};