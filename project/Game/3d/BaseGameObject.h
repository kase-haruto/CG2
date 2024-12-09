#pragma once

//* engine *//
#include "Engine/objects/Model.h"

//* c++ lib *//
#include <memory>
#include <string>

class BaseGameObject{
public:
	//===================================================================*/
	//                    public methods
	//===================================================================*/
	BaseGameObject(const std::string& modelName);
	BaseGameObject() = default;
	virtual ~BaseGameObject();

	virtual void Initialize();
	virtual void Update();
	virtual void Draw();
	virtual void ImGui();
	virtual void ShowDebugUI();

private:
	//===================================================================*/
	//                    public methods
	//===================================================================*/
	std::string objectName_;

protected:
	//===================================================================*/
	//                    protected methods
	//===================================================================*/
	std::unique_ptr<Model> model_ = nullptr;	// 描画用モデル

private:
	//===================================================================*/
	//                    private methods
	//===================================================================*/
	static std::vector<BaseGameObject*> allGameObjects_;
	std::string jsonPath = "gameobject";

public:
	//===================================================================*/
	//                    getter/setter
	//===================================================================*/
	void SetName(const std::string& name);
	const std::string& GetName()const{ return objectName_; }
	static const std::vector<BaseGameObject*>& GetGameObjects();

};

