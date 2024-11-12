#pragma once

#include "Game/objects/Enemy.h"

class Boss:public Enemy{
public:
	Boss() = default;
	~Boss()override;

	void Initialize(Model* model, const Vector3& position);

	void Update()override;

	void Draw()override;

	void Reset();

	void OnCollision(Collider* other)override;

	void SetIsSpawn(bool frag){ isSpawn_ = frag; }

private:
	void Move();

private:
	int life_ = 80;

	bool isSpawn_ = false;
};
