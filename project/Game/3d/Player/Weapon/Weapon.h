#pragma once

// engine
#include "../../BaseGameObject.h"

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

private:
	//===================================================================*/
	//                   private func
	//===================================================================*/

private:
	//===================================================================*/
	//                   private variables
	//===================================================================*/
	Vector3 offset_ = {0.0f, 0.0f, 0.0f};	//< オフセット
	Vector3 tipPos_ = {0.0f, 0.0f, 0.0f};	//< 先端座標

public:
	//===================================================================*/
	//                   getter
	//===================================================================*/
	const Vector3& GetOffset()const{ return offset_; }
	const Vector3& GetTipPos()const{ return tipPos_; }

	const Vector3 GetCenterPos()const override{
		const Vector3 offset = {0.0f, 0.5f, 0.0f};
		Vector3 worldPos = Vector3::Transform(offset, model_->worldMatrix);
		return worldPos;
	}

	void SetPosition(const Vector3& pos){
		model_->transform.translate = pos;
	}
};

