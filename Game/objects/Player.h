#pragma once

#include "objects/Character.h"

class Player
	:public Character{
public:
	Player() = default;
	~Player()override = default;


	/// <summary>
	/// ‰Šú‰»
	/// </summary>
	/// <param name="model"></param>
	void Initialize(Model* model)override;

	/// <summary>
	/// XV
	/// </summary>
	void Update()override;

	/// <summary>
	/// •`‰æ
	/// </summary>
	void Draw()override;

private:

};
