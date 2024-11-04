#pragma once

/* engine */
#include "objects/Collider.h"
#include "objects/Model.h"
#include "graphics/ViewProjection.h"

class Character :
	public Collider{
public:
	Character() = default;
	virtual ~Character()override;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(Model* model);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// viewProjectionをセット
	/// </summary>
	/// <param name="viewProjection"></param>
	void SetViewProjection(const ViewProjection* viewProjection);

	/// <summary>
	/// トランスフォームを取得
	/// </summary>
	/// <returns></returns>
	const Transform& GetTransform()const{ return model_->transform; }

	virtual void OnCollision(Collider* other)override{};
	virtual const Vector3 GetCenterPos()const override{
		const Vector3 offset = {0.0f,1.5f,0.0f};
		Vector3 worldPos = Matrix4x4::Transform(offset, model_->worldMatrix);
		return worldPos;
	};

protected:
	//描画モデル
	Model* model_ = nullptr;

	const ViewProjection* pViewProjection_;

};
