#pragma once
#include <string>

enum class ObjectType{
	Camera,			//カメラ
	Light,			//ライト
	GameObject,		//ゲームオブジェクト
	None,			//なし
};

class SceneObject{
public:
	//==================================================================*//
	//			public functions
	//==================================================================*//
	virtual ~SceneObject() = default;
	SceneObject();
	virtual void ShowGui();

protected:
	//==================================================================*//
	//			protected functions
	//==================================================================*//
	std::string name_;		//オブジェクト名
	ObjectType objectType_;	//オブジェクトの種類

public:
	//==================================================================*//
	//			getter/setter
	//==================================================================*//
	virtual void SetName(const std::string& name,ObjectType type){
		name_ = name;
		objectType_ = type;
	}

	const std::string& GetName()const{ return name_; }
	ObjectType GetObjectType()const{ return objectType_; }
};