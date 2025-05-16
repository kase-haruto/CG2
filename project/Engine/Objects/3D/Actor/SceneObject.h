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
	virtual void RegisterToRenderer(class MeshRenderer*){}

	virtual void SaveConfig([[maybe_unused]]const std::string& path) {}
	virtual void LoadConfig([[maybe_unused]] const std::string& path) {}
	virtual void ApplyConfig() {}

	void EnableGuiList(); //GUIのリストに追加するかどうか

	// accessor =======================================================*//
	virtual void SetName(const std::string& name, ObjectType type);

	const std::string& GetName()const{ return name_; }
	ObjectType GetObjectType()const{ return objectType_; }

	void SetConfigPath(const std::string& path) {configPath_ = path;}

protected:
	//==================================================================*//
	//			protected functions
	//==================================================================*//
	std::string name_;				//オブジェクト名
	ObjectType objectType_;			//オブジェクトの種類

	WorldTransform worldTransform_;	//ワールドトランスフォーム
protected:
	std::string configPath_ = "Resources/Configs/";
};