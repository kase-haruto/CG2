#pragma once

#include "Engine/objects/Collider/Collider.h"

// c++
#include <list>
#include <array>
#include <vector>

class CollisionManager{
public:
	using CollisionShape = std::variant<Sphere, OBB>;
	//===================================================================*/
	//                   singleton
	//===================================================================*/
	static CollisionManager* GetInstance();
	CollisionManager(const CollisionManager&) = delete;
	CollisionManager& operator=(const CollisionManager&) = delete;

public:
	//===================================================================*/
	//                   public functions
	//===================================================================*/
	void UpdateCollisionAllCollider();			// すべてのコライダーを総当たりで判定

	void AddCollider(Collider* collider);		// コライダーリストに追加
	void RemoveCollider(Collider* collider);	// コライダーリストから削除
	void DebugLog();

private:
	//===================================================================*/
	//                   private functions
	//===================================================================*/
	CollisionManager() = default;
	~CollisionManager() = default;

	bool CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	/*----------------
	 各形状ごとの衝突
	----------------*/
	bool SphereToSphere(const Sphere& sphereA, const Sphere& sphereB);
	bool SphereToOBB(const Sphere& sphere, const OBB obb);
	bool OBBToOBB(const OBB& obbA, const OBB& obbB);


private:
	//===================================================================*/
	//                   private variable
	//===================================================================*/
	std::list<Collider*>colliders_;
	std::vector<std::string> collisionLogs_; // 衝突ログ
};

