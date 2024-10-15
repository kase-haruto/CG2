#pragma once

#include "objects/Character.h"

class Player
	:public Character{
public:
	Player() = default;
	~Player()override = default;


	/// <summary>
	/// ������
	/// </summary>
	/// <param name="model"></param>
	void Initialize(Model* model)override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update()override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw()override;

private:

};
