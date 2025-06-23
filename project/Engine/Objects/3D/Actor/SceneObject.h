#pragma once

#include <Engine/objects/Transform/Transform.h>
#include <Engine/objects/Collider/Collider.h>
#include <Engine/objects/ConfigurableObject/ConfigurableObject.h>
#include <Engine/Objects/3D/Geometory/AABB.h>

#include <string>
#include <nlohmann/json.hpp>

enum class ObjectType {
	Camera,			//カメラ
	Light,			//ライト
	GameObject,		//ゲームオブジェクト
	ParticleSystem, //パーティクルシステム
	None,			//なし
};

class IConfigurable; // 前方宣言

class SceneObject {
public:
	//==================================================================*//
	//				public functions
	//==================================================================*//
	virtual ~SceneObject() = default;
	SceneObject();

	virtual void Update() = 0;
	virtual void ShowGui();
	virtual void RegisterToRenderer(class MeshRenderer*) {}
	virtual AABB GetWorldAABB() const { return FallbackAABBFromTransform(); }

	AABB FallbackAABBFromTransform()const;
	//==================================================================*//
	//				Config I/O virtuals
	//==================================================================*//
	virtual std::string GetObjectTypeName() const;
	virtual void SetName(const std::string& name, ObjectType type);
	void SetConfigPath(const std::string& path) { configPath_ = path; }

	//==================================================================*//
	//				SceneObject I/O helpers
	//==================================================================*//
	virtual bool IsSerializable() const { return true; }
	virtual bool HasConfigInterface() const;

	//==================================================================*//
	//				Accessors
	//==================================================================*//
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	WorldTransform& GetWorldTransform() { return worldTransform_; }
	ObjectType GetObjectType()  const { return objectType_; }
	const std::string& GetName()        const { return name_; }
	const std::string& GetConfigPath()  const { return configPath_; }

	bool IsEnableRaycast() const { return isEnableRaycast_; }
	void SetEnableRaycast(bool enable) { isEnableRaycast_ = enable; }

	// 親子関係
	SceneObject* GetParent() const{ return parent_; }
	const std::vector<SceneObject*>& GetChildren() const{ return children_; }
	void SetParent(SceneObject* newParent);

	void UpdateWorldTransformRecursive();

protected:
	std::string name_ = "";          // オブジェクト名
	ObjectType  objectType_ = ObjectType::None;
	WorldTransform worldTransform_;             // ワールドトランスフォーム
	// 親子構造
	SceneObject* parent_ = nullptr;
	std::vector<SceneObject*> children_;

	// 設定ファイル保存先パス
	std::string configPath_ = "";

protected:
	bool isEnableRaycast_ = true; // レイキャストを有効にするかどうか
};