#include "Collision/CollisionManager.h"

#include <cmath>

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