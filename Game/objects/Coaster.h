#pragma once

#include "objects/Model.h"

class Coaster{
public:
	Coaster();
	~Coaster();

	void Initialize();

	void Update();

	void Draw();
	
private:
	//プレイヤー
	Transform* targetTransform_ = nullptr;
	//transform
	Transform transform_;
	//対象からのオフセット
	Vector3 offset_;
	//描画用モデル
	Model* model_ = nullptr;
};

