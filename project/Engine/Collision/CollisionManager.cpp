#include "CollisionManager.h"

#include "lib/myFunc/MyFunc.h"

#include <externals/imgui/imgui.h>

#include <algorithm>

CollisionManager* CollisionManager::GetInstance(){
	static CollisionManager instance;
	return &instance;
}

std::string CollisionManager::MakeCollisionKey(Collider* colliderA, Collider* colliderB){

	// 名前を使ってユニークなキーを生成（順序を保証するために名前をソート）
	return (colliderA->GetName() < colliderB->GetName())
		? colliderA->GetName() + " VS " + colliderB->GetName()
		: colliderB->GetName() + " VS " + colliderA->GetName();

}

// ヘルパー関数: 衝突ペアがログを記録すべきかを判定
bool CollisionManager::ShouldLogCollision(const Collider* a, const Collider* b){
	// aのターゲットタイプにbのタイプが含まれているか
	bool aWantsToCollideWithB = (a->GetTargetType() & b->GetType()) != ColliderType::Type_None;

	// bのターゲットタイプにaのタイプが含まれているか
	bool bWantsToCollideWithA = (b->GetTargetType() & a->GetType()) != ColliderType::Type_None;

	return aWantsToCollideWithB || bWantsToCollideWithA;
}

void CollisionManager::UpdateCollisionAllCollider(){
	// 前のフレームの衝突を保存してリセット
	previousCollisions_ = std::move(currentCollisions_);
	currentCollisions_.clear();

	for (auto itA = colliders_.begin(); itA != colliders_.end(); ++itA){
		// コライダーがアクティブでなければスキップ
		if (!(*itA)->IsActive()) continue;

		for (auto itB = std::next(itA); itB != colliders_.end(); ++itB){
			// コライダーがアクティブでなければスキップ
			if (!(*itB)->IsActive()) continue;

			if (CheckCollisionPair(*itA, *itB)){
				// 衝突ペアのキーを生成
				std::string key = MakeCollisionKey(*itA, *itB);
				currentCollisions_.insert(key);

				// 衝突状態をログに記録
				if (previousCollisions_.find(key) == previousCollisions_.end()){
					// 新しい衝突 (Enter)
					(*itA)->OnCollisionEnter(*itB);
					(*itB)->OnCollisionEnter(*itA);
					collisionLogs_.emplace_back("Enter: " + key);
				} else{
					// 持続中の衝突 (Stay)
					(*itA)->OnCollisionStay(*itB);
					(*itB)->OnCollisionStay(*itA);
				}
			}
		}
	}

	// 前のフレームに存在して現在のフレームに存在しないペアをExitとして記録
	for (const auto& key : previousCollisions_){
		if (currentCollisions_.find(key) == currentCollisions_.end()){
			// ペアを分解して対応するコライダーを取得
			auto delimiterPos = key.find('-');
			std::string colliderAName = key.substr(0, delimiterPos);
			std::string colliderBName = key.substr(delimiterPos + 1);

			Collider* colliderA = FindColliderByName(colliderAName);
			Collider* colliderB = FindColliderByName(colliderBName);

			if (colliderA && colliderB){
				colliderA->OnCollisionExit(colliderB);
				colliderB->OnCollisionExit(colliderA);
			}

			// Exitログを記録
			collisionLogs_.emplace_back("Exit: " + key);
		}
	}
}

Collider* CollisionManager::FindColliderByName(const std::string& name){
	for (auto* collider : colliders_){
		if (collider->GetName() == name){
			return collider;
		}
	}
	return nullptr; // 見つからない場合
}



void CollisionManager::AddCollider(Collider* collider){

	colliders_.emplace_back(collider);

}

void CollisionManager::RemoveCollider(Collider* collider){

	colliders_.remove(collider);

}

void CollisionManager::DebugLog(){

	// 衝突数を表示
	ImGui::Text("Colliders count: %zu", colliders_.size());
	ImGui::Text("Collisions detected: %zu", currentCollisions_.size());

	// スクロール可能なログフィールド
	ImGui::BeginChild("LogScroll", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
	for (const auto& log : collisionLogs_){
		ImGui::TextUnformatted(log.c_str());
	}
	ImGui::EndChild();

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

	// Sphereの中心とOBBの中心の差分
	Vector3 diff = sphereCenter - obb.center;
	Vector3 closestPoint = obb.center;

	// 各軸でクランプ処理
	for (int i = 0; i < 3; ++i){
		// diffをOBB軸方向に投影して距離を取得
		float distance = Vector3::Dot(diff, obbAxes[i]);
		// OBBの半サイズ（軸方向の幅）
		float halfExtent = (i == 0) ? obb.size.x * 0.5f :
			(i == 1) ? obb.size.y * 0.5f :
			obb.size.z * 0.5f;

		// クランプして最近接点を計算
		distance = std::clamp(distance, -halfExtent, halfExtent);
		closestPoint += distance * obbAxes[i];
	}

	// 最近接点とSphere中心の距離を計算
	Vector3 closestToSphere = closestPoint - sphereCenter;
	float distanceSquared = closestToSphere.LengthSquared();

	// 衝突判定
	return distanceSquared <= (sphere.radius * sphere.radius);
}

bool CollisionManager::OBBToOBB([[maybe_unused]] const OBB& obbA, [[maybe_unused]] const OBB& obbB){
	// 1) A, B それぞれの 3軸ベクトル を求める
	Vector3 aAxes[3];
	Vector3 bAxes[3];
	ComputeOBBAxes(obbA, aAxes);
	ComputeOBBAxes(obbB, bAxes);

	// 2) 下記の全ての軸について OverlapOnAxis をチェック
	//    (1) Aの軸3本
	for (int i = 0; i < 3; i++){
		if (!OverlapOnAxis(obbA, aAxes, obbB, bAxes, aAxes[i])){
			return false; // 重ならない -> 分離軸
		}
	}
	//    (2) Bの軸3本
	for (int i = 0; i < 3; i++){
		if (!OverlapOnAxis(obbA, aAxes, obbB, bAxes, bAxes[i])){
			return false;
		}
	}
	//    (3) A.axis[i] × B.axis[j]  (i,j in [0..2])
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			Vector3 crossAxis = Vector3::Cross(aAxes[i], bAxes[j]);
			if (!OverlapOnAxis(obbA, aAxes, obbB, bAxes, crossAxis)){
				return false;
			}
		}
	}

	// 3) すべての軸で重なった -> 衝突している
	return true;
}


void CollisionManager::ComputeOBBAxes(const OBB& obb, Vector3 outAxis[3]){
	Matrix4x4 rot = EulerToMatrix(obb.rotate);

	// 行列の1行(or1列)が軸ベクトルになる
	// 行列の並びは実装に応じて違うので要調整
	// ここでは row-major として書く一例
	outAxis[0] = Vector3(rot.m[0][0], rot.m[0][1], rot.m[0][2]); // X軸
	outAxis[1] = Vector3(rot.m[1][0], rot.m[1][1], rot.m[1][2]); // Y軸
	outAxis[2] = Vector3(rot.m[2][0], rot.m[2][1], rot.m[2][2]); // Z軸

	// 念のため正規化 (回転行列なら本来長さ1だが、積算順序等で誤差が出るかも)
	outAxis[0].Normalize();
	outAxis[1].Normalize();
	outAxis[2].Normalize();
}

float CollisionManager::ProjectOBB(const OBB& obb, const Vector3 obbAxes[3], const Vector3& axisCandidate){
	// OBBの半サイズ(各軸方向の半径)
	Vector3 halfSize = obb.size * 0.5f;

	// 3つの軸に投影して絶対値を足し合わせる
	float r =
		fabs(halfSize.x * Vector3::Dot(obbAxes[0],axisCandidate)) +
		fabs(halfSize.y * Vector3::Dot(obbAxes[1], axisCandidate)) +
		fabs(halfSize.z * Vector3::Dot(obbAxes[2], axisCandidate));

	return r;
}

bool CollisionManager::OverlapOnAxis(const OBB& obbA, const Vector3 aAxes[3], const OBB& obbB, const Vector3 bAxes[3], const Vector3& axisCandidate){
	// 軸が正規化されていないなら正規化しておく
	Vector3 axis = axisCandidate;
	float lenSq = axis.LengthSquared();
	if (lenSq < 1e-8f){
		// 軸がほぼゼロベクトルの場合は別の軸としてスキップ or 重なっているとみなす
		return true;
	}
	axis.Normalize();

	// A, B それぞれの投影中心(スカラー値)
	float centerA = Vector3::Dot(obbA.center, axis);
	float centerB = Vector3::Dot(obbB.center, axis);

	// A, B それぞれの投影半径
	float rA = ProjectOBB(obbA, aAxes, axis);
	float rB = ProjectOBB(obbB, bAxes, axis);

	// 中心距離
	float dist = fabs(centerB - centerA);

	// dist が rA + rB より大きい → 投影区間が重ならない
	return (dist <= (rA + rB));
}
