#pragma once

#include "objects/Model.h"
#include "graphics/ViewProjection.h"

class Character{
public:
	Character() = default;
	virtual ~Character() = default;

	/// <summary>
	/// ‰Šú‰»
	/// </summary>
	virtual void Initialize(Model* model);

	/// <summary>
	/// XV
	/// </summary>
	virtual void Update();

	/// <summary>
	/// •`‰æ
	/// </summary>
	virtual void Draw();
	
	void SetViewProjection(const ViewProjection* viewProjection);

private:
	//•`‰æƒ‚ƒfƒ‹
	Model* model_ = nullptr;
};
