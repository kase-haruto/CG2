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
	void ImGui();
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
	static std::vector<BaseGameObject*> allGameObjects_;

public:
	//===================================================================*/
	//                    getter/setter
	//===================================================================*/
	void SetName(const std::string& name);
	const std::string& GetName()const{ return objectName_; }
	static const std::vector<BaseGameObject*>& GetGameObjects();

};

