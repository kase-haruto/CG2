#include "Collision/CollisionManager.h"

#include <cmath>
#include <algorithm>

CollisionManager* CollisionManager::GetInstance(){
	static CollisionManager* instance;
	if (!instance){
		instance = new CollisionManager();
	}
	return instance;
}

CollisionManager::CollisionManager(){
    Reset();
}

CollisionManager::~CollisionManager(){

}

void CollisionManager::Reset(){
    colliders_.clear();
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB){
    Vector3 colliderA_pos = colliderA->GetCenterPos();
    Vector3 colliderB_pos = colliderB->GetCenterPos();
    float colliderA_radius = colliderA->GetRadius();
    float colliderB_radius = colliderB->GetRadius();

    Vector3 sub = colliderB_pos - colliderA_pos;
    float distance = sub.Length();

    if (distance <= colliderA_radius + colliderB_radius){
        colliderA->OnCollision(colliderB);
        colliderB->OnCollision(colliderA);
    }
}

void CollisionManager::CheckCollisionSphereToLine(const Segment& line, Collider* collider){
    Vector3 sphereCenter = collider->GetCenterPos();
    float sphereRadius = collider->GetRadius();

    // 線分の始点と終点
    Vector3 lineStart = line.startPos;
    Vector3 lineEnd = line.endPos;

    // 線分のベクトルと線分上の最近接点を計算
    Vector3 lineVec = lineEnd - lineStart;
    Vector3 startToCenter = sphereCenter - lineStart;

    // 内積で線分上の最短距離を計算
    float t = startToCenter.Dot(lineVec) / lineVec.Dot(lineVec);
    t = std::clamp(t, 0.0f, 1.0f); // 線分の範囲内に制限

    // 線分上の最近接点
    Vector3 closestPoint = lineStart + lineVec * t;

    // 球の中心と最近接点との距離を計算
    Vector3 toClosestPoint = sphereCenter - closestPoint;
    float distance = toClosestPoint.Length();

    // 衝突判定
    if (distance <= sphereRadius){
        collider->OnCollision(nullptr); // 線分はコライダーでないので nullptr を渡す
    }
}


void CollisionManager::CheckAllCollidion(){

    for (auto itrA = colliders_.begin(); itrA != colliders_.end(); ++itrA){
        Collider* colliderA = *itrA;

        for (auto itrB = std::next(itrA); itrB != colliders_.end(); ++itrB){
            Collider* colliderB = *itrB;

            CheckCollisionPair(colliderA, colliderB);
        }
    }
}

void CollisionManager::AddCollider(Collider* collider){
    colliders_.push_back(collider);
}

void CollisionManager::RemoveCollider(Collider* collider){
    colliders_.remove(collider);  // リストから削除
}