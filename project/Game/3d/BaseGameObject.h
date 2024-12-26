#pragma once

//* engine *//
#include "Engine/objects/Model/Model.h"
#include "Engine/objects/SceneObject.h"

//* c++ lib *//
#include <memory>
#include <string>

class BaseGameObject
	:public SceneObject{
public:
	//===================================================================*/
	//                    public methods
	//===================================================================*/
	BaseGameObject(const std::string& modelName);
	BaseGameObject() = default;
	virtual ~BaseGameObject()override;

	virtual void Initialize();
	virtual void Update();
	virtual void Draw();
	virtual void ShowGui()override;

protected:
	//===================================================================*/
	//                    protected methods
	//===================================================================*/
	std::unique_ptr<Model> model_ = nullptr;	// 描画用モデル

protected:
	//===================================================================*/
	//                    private methods
	//===================================================================*/
	std::string jsonPath = "gameobject";

public:
	//===================================================================*/
	//                    getter/setter
	//===================================================================*/
	void SetName(const std::string& name);
	const std::string& GetName()const{ return name_; }

	virtual const Vector3 GetCenterPos()const{
		const Vector3 offset = {0.0f, 1.0f, 0.0f};
		Vector3 worldPos = Vector3::Transform(offset, model_->worldMatrix);
		return worldPos;
	}
};

