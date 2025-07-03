#pragma once

#include <string>
#include <vector>
#include <externals/nlohmann/json.hpp>

#include <Engine/objects/Transform/Transform.h>
#include <Engine/objects/Collider/Collider.h>
#include <Engine/Objects/3D/Geometory/AABB.h>
#include <Engine/Foundation/Utility/Guid/Guid.h>

enum class ObjectType{
	Camera,         // カメラ
	Light,          // ライト
	GameObject,     // ゲームオブジェクト
	ParticleSystem, // パーティクルシステム
	None,           // なし
};

class IConfigurable; // 前方宣言

class SceneObject{
public:
	// =======================
	// Constructors & Destructor
	// =======================
	SceneObject();
	virtual ~SceneObject() = default;

	// =======================
	// Main Interface
	// =======================
	virtual void Update() = 0;

	// 描画コマンドリストは使わない場合もあるので maybe_unused 指定
	virtual void Draw([[maybe_unused]] ID3D12GraphicsCommandList* cmdList){}

	virtual void ShowGui();

	// =======================
	// Bounding Volume
	// =======================
	virtual AABB GetWorldAABB() const{ return FallbackAABBFromTransform(); }

	// トランスフォームから簡易AABBを計算
	AABB FallbackAABBFromTransform() const;

	// =======================
	// Config I/O virtuals
	// =======================
	virtual std::string GetObjectTypeName() const;

	virtual void SetName(const std::string& name, ObjectType type);

	void SetConfigPath(const std::string& path){ configPath_ = path; }

	// =======================
	// Serialization and Config Interface
	// =======================
	virtual bool IsSerializable() const{ return true; }

	virtual bool HasConfigInterface() const;

	// =======================
	// Accessors
	// =======================
	const WorldTransform& GetWorldTransform() const{ return worldTransform_; }
	WorldTransform& GetWorldTransform(){ return worldTransform_; }

	ObjectType GetObjectType() const{ return objectType_; }

	const std::string& GetName() const{ return name_; }
	const std::string& GetConfigPath() const{ return configPath_; }

	bool IsEnableRaycast() const{ return isEnableRaycast_; }
	void SetEnableRaycast(bool enable){ isEnableRaycast_ = enable; }

	bool IsDrawEnable() const{ return isDrawEnable_; }
	virtual void SetDrawEnable(bool enable){ isDrawEnable_ = enable; }

	SceneObject* GetParent() const{ return parent_; }
	const std::vector<SceneObject*>& GetChildren() const{ return children_; }

	virtual void SetParent(SceneObject* newParent);

	const Guid& GetGuid() const{ return id_; }
	void SetGuid(const Guid& g){ id_ = g; }

	void UpdateWorldTransformRecursive();

protected:
	// =======================
	// Identification
	// =======================
	std::string name_ = "";				// オブジェクト名
	std::string configPath_ = "";		// コンフィグファイルパス
	Guid id_;							// 識別子
	Guid parentId_;
	ObjectType objectType_ = ObjectType::None;

	// =======================
	// Transform & Hierarchy
	// =======================
	WorldTransform worldTransform_;			// ワールドトランスフォーム
	SceneObject* parent_ = nullptr;			// 親オブジェクト
	std::vector<SceneObject*> children_;	// 子オブジェクトリスト

	// =======================
	// State Flags
	// =======================
	bool isEnableRaycast_ = true;   // レイキャスト有効/無効
	bool isDrawEnable_ = true;      // 描画有効/無効
};
