#pragma once

#include <Engine/objects/Transform/Transform.h>

#include <string>

enum class ObjectType {
	Camera,			//カメラ
	Light,			//ライト
	GameObject,		//ゲームオブジェクト
	None,			//なし
};

class SceneObject {
public:
	//==================================================================*//
	//            public functions
	//==================================================================*//
	virtual ~SceneObject() = default;
	SceneObject();

	virtual void Update() = 0;
	virtual void ShowGui();
	virtual void RegisterToRenderer(class MeshRenderer*) {}

	//==================================================================*//
	//            Config I/O virtuals
	//==================================================================*//
	virtual void ApplyConfigFromJson([[maybe_unused]]const nlohmann::json& j) {};
	virtual void ExtractConfigToJson([[maybe_unused]] nlohmann::json& j) const {};
	virtual std::string GetObjectTypeName() const;
	virtual void SetName(const std::string& name, ObjectType type);
	void SetConfigPath(const std::string& path) { configPath_ = path; }

	//==================================================================*//
	//            SceneObject I/O helpers
	//==================================================================*//
	virtual void ApplyConfig() = 0;
	virtual void LoadConfig(const std::string& path);
	virtual void SaveConfig(const std::string& path) const;

	//==================================================================*//
	//            Accessors
	//==================================================================*//
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	WorldTransform& GetWorldTransform() { return worldTransform_; }
	ObjectType GetObjectType()  const { return objectType_; }
	const std::string& GetName()        const { return name_; }
	const std::string& GetConfigPath()  const { return configPath_; }

protected:
	std::string name_ = "";          // オブジェクト名
	ObjectType  objectType_ = ObjectType::None;
	WorldTransform worldTransform_;             // ワールドトランスフォーム

	// 設定ファイル保存先パス
	std::string configPath_ = "";
};