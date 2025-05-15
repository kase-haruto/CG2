#pragma once

#include <Engine/objects/Transform/Transform.h>

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
	virtual ~SceneObject();
	SceneObject();
	virtual void ShowGui();
	virtual void Update() =0;
	void EnableGuiList(); //GUIのリストに追加するかどうか

	// accessor =======================================================*//
	virtual void SetName(const std::string& name, ObjectType type){
		name_ = name;
		objectType_ = type;
	}

	const std::string& GetName()const{ return name_; }
	ObjectType GetObjectType()const{ return objectType_; }

protected:
	//==================================================================*//
	//			protected functions
	//==================================================================*//
	std::string name_;				//オブジェクト名
	ObjectType objectType_;			//オブジェクトの種類

	WorldTransform worldTransform_;	//ワールドトランスフォーム
};