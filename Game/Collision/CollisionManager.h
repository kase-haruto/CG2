#pragma once 

#include"objects/Collider.h"
#include "Vector3.h"

/* lib */
#include<list>
#include<memory>

class CollisionManager{
public:
    /// <summary>
    /// 二つのコライダーの衝突時の応答
    /// </summary>
    void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

    /// <summary>
    /// 総当たりの衝突判定
    /// </summary>
    void CheckAllCollidion();

    /// <summary>
    /// コライダーの追加
    /// </summary>
    void AddCollider(Collider* collider);

    /// <summary>
    /// コライダーの削除
    /// </summary>
    /// <param name="collider"></param>
    void RemoveCollider(Collider* collider);

    /// <summary>
    /// コライダーのリセット
    /// </summary>
    void Reset();

private:
	CollisionManager();
	~CollisionManager();
	CollisionManager(const CollisionManager&) = delete;
	CollisionManager& operator= (const CollisionManager&) = delete;

public:

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns></returns>
	static CollisionManager* GetInstance();

private:
	//オブジェクトリスト
	std::list<Collider*>colliders_;
};

