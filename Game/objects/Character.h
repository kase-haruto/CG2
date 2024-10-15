#pragma once

#include "objects/Model.h"
#include "graphics/ViewProjection.h"

class Character{
public:
	Character() = default;
	virtual ~Character() = default;

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize(Model* model);

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update();

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw();
	
	void SetViewProjection(const ViewProjection* viewProjection);

private:
	//�`�惂�f��
	Model* model_ = nullptr;
};
