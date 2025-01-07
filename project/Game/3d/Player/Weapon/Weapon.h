#pragma once

// engine
#include "../../BaseGameObject.h"
#include "lib/myFunc/MyFunc.h"

class Weapon :
	public BaseGameObject{
public:
	//===================================================================*/
	//                   public func
	//===================================================================*/
	Weapon() = default;
	Weapon(const std::string& modelName);
	~Weapon()override = default;

	void Initialize()override;
	void Update()override;
	void Draw()override;
	void ShowGui()override;
	void InitializeTransform();
private:
	//===================================================================*/
	//                   private func
	//===================================================================*/

private:
	//===================================================================*/
	//                   private variables
	//===================================================================*/
	Vector3 offset_ = {0.0f, 0.0f, 0.0f};	//< オフセット
	Vector3 tipPos_ = {0.0f, 2.0f, 0.0f};	//< 先端座標

public:
	//===================================================================*/
	//                   getter
	//===================================================================*/
	const Vector3& GetOffset()const{ return offset_; }
	const Vector3 GetTipPos()const{ return ComputeTipWorldPosition(); }

	const Vector3 GetCenterPos()const override{
		const Vector3 offset = {0.0f, 0.5f, 0.0f};
		Vector3 worldPos = Vector3::Transform(offset, model_->worldMatrix);
		return worldPos;
	}

	Vector3 ComputeTipWorldPosition() const;

	const Vector3& GetRotate()const{
		return model_->transform.rotate;
	}

	void SetPosition(const Vector3& pos){
		model_->transform.translate = pos;
	}

	void SetRotate(const Vector3& rotate){
		model_->transform.rotate = rotate;
	}

	Matrix4x4 GetWorldMatrix()const{
		return model_->worldMatrix;
	}
};

