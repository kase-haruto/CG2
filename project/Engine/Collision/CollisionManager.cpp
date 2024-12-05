#include "CollisionManager.h"

#include "lib/myFunc/MyFunc.h"

#include <algorithm>

CollisionManager* CollisionManager::GetInstance(){
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::UpdateCollisionAllCollider(){

	for (auto itA = colliders_.begin(); itA != colliders_.end(); ++itA){
		for (auto itB = std::next(itA); itB != colliders_.end(); ++itB){

			if (CheckCollisionPair(*itA, *itB)){
				(*itA)->OnCollision(*itB);
				(*itB)->OnCollision(*itA);
			}

		}
	}

}

void CollisionManager::AddCollider(Collider* collider){

	colliders_.emplace_back(collider);

}

void CollisionManager::RemoveCollider(Collider* collider){

	colliders_.remove(collider);

}

bool CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB){

	auto shapeA = colliderA->GetCollisionShape();
	auto shapeB = colliderB->GetCollisionShape();

	if (shapeA.index() > shapeB.index()){
		std::swap(shapeA, shapeB);
	}

	return std::visit(
		[&] (const auto& shapeA, const auto& shapeB) -> bool{
			using ShapeTypeA = std::decay_t<decltype(shapeA)>;
			using ShapeTypeB = std::decay_t<decltype(shapeB)>;

			//===================================================================*/
			//                    Sphere vs Sphere
			//===================================================================*/
			if constexpr (std::is_same_v<ShapeTypeA, Sphere> && std::is_same_v<ShapeTypeB, Sphere>){

				return SphereToSphere(shapeA, shapeB);

			}

			//===================================================================*/
			//                    Sphere vs Obb
			//===================================================================*/
			else if constexpr (std::is_same_v<ShapeTypeA, Sphere> && std::is_same_v<ShapeTypeB, OBB>){

				return SphereToOBB(shapeA, shapeB);

			}

			//===================================================================*/
			//                    OBB vs OBB
			//===================================================================*/
			else if constexpr (std::is_same_v<ShapeTypeA, OBB> && std::is_same_v<ShapeTypeB, OBB>){

				return OBBToOBB(shapeA, shapeB);

			} 

			// 設定していない組み合わせ
			else{

				return false;
			
			}

		}, shapeA, shapeB);
}


bool CollisionManager::SphereToSphere(const Sphere& sphereA, const Sphere& sphereB){
	const Vector3& centerA = sphereA.center;
	const Vector3& centerB = sphereB.center;
	float radiusSum = sphereA.radius + sphereB.radius;

	// 中心間距離の2乗を計算
	Vector3 diff = centerA - centerB;
	float distanceSquared = diff.LengthSquared();

	// 衝突判定
	return distanceSquared <= (radiusSum * radiusSum);


}

bool CollisionManager::SphereToOBB(const Sphere& sphere, const OBB obb){
	// Sphereの中心
	const Vector3& sphereCenter = sphere.center;

	// OBBの軸方向（X, Y, Z）の取得
	Matrix4x4 rotationMatrix = EulerToMatrix(obb.rotate);
	Vector3 obbAxes[3] = {
		Vector3::Transform(Vector3(1.0f, 0.0f, 0.0f), rotationMatrix),
		Vector3::Transform(Vector3(0.0f, 1.0f, 0.0f), rotationMatrix),
		Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), rotationMatrix),
	};

	// Sphereの中心をOBBのローカル座標系に変換
	Vector3 localSphereCenter = sphereCenter - obb.center;
	Vector3 closestPoint = obb.center;

	// 各軸でクランプ
	for (int i = 0; i < 3; ++i){
		float distance = Vector3::Dot(localSphereCenter, obbAxes[i]);
		float halfExtent = (i == 0) ? obb.size.x : (i == 1) ? obb.size.y : obb.size.z;

		// 距離をクランプ
		distance = std::clamp(distance, -halfExtent, halfExtent);
		closestPoint += distance * obbAxes[i];
	}

	// 最短距離を計算
	Vector3 diff = closestPoint - sphereCenter;
	float distanceSquared = diff.LengthSquared();

	// 衝突判定
	return distanceSquared <= (sphere.radius * sphere.radius);
}

bool CollisionManager::OBBToOBB([[maybe_unused]] const OBB& obbA, [[maybe_unused]] const OBB& obbB){
	return false;
}