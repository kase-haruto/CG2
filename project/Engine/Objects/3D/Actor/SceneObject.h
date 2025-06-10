#pragma once

#include <Engine/objects/Transform/Transform.h>
#include <Engine/objects/Collider/Collider.h>
#include <Engine/Objects/3D/Geometory/AABB.h>

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
protected:
	std::string name_ = "";          // オブジェクト名
	ObjectType  objectType_ = ObjectType::None;
	WorldTransform worldTransform_;             // ワールドトランスフォーム

	// 設定ファイル保存先パス
	std::string configPath_ = "";

protected:
	bool isEnableRaycast_ = true; // レイキャストを有効にするかどうか
};