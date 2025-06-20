#include "Raycastor.h"

#include <Engine/Foundation/Utility/Func/MyFunc.h>

bool IntersectRayAABB(const Ray& ray, const AABB& aabb, float& tOut) {
	float tmin = 0.0f;
	float tmax = tOut;

	for (int i = 0; i < 3; ++i) {
		float invD = 1.0f / ray.direction[i];
		float t0 = (aabb.min_[i] - ray.origin[i]) * invD;
		float t1 = (aabb.max_[i] - ray.origin[i]) * invD;
		if (invD < 0.0f) std::swap(t0, t1);

		tmin = (std::max)(tmin, t0);
		tmax = (std::min)(tmax, t1);
		if (tmax < tmin)
			return false;
	}
	tOut = tmin;
	return true;
}

std::optional<RaycastHit> Raycastor::Raycast(const Ray& ray, const std::vector<SceneObject*>& objects, float maxDistance) {
	std::optional<RaycastHit> closestHit;

	for (auto* obj : objects) {
		if (!obj) continue;
		if (!obj->IsEnableRaycast()) continue;
		AABB box = obj->GetWorldAABB();

		float t = maxDistance;
		if (IntersectRayAABB(ray, box, t) && t < maxDistance) {
			closestHit = RaycastHit{
				.distance = t,
				.point = ray.origin + ray.direction * t,
				.normal = Vector3(),
				.hitObject = obj
			};
			maxDistance = t;
		}
	}
	return closestHit;
}

Ray Raycastor::ConvertMouseToRay(const Vector2& mousePos, const Matrix4x4& view, const Matrix4x4& proj, const Vector2& viewportSize){
	// 1. NDC座標に変換
	float ndcX = (2.0f * mousePos.x) / viewportSize.x - 1.0f;
	float ndcY = 1.0f - (2.0f * mousePos.y) / viewportSize.y; // Y反転（DirectX準拠）

	// 2. 近接点 (z=0) と遠方点 (z=1) をクリップ空間で作成
	Vector4 nearPoint = Vector4(ndcX, ndcY, 0.0f, 1.0f); // near plane
	Vector4 farPoint = Vector4(ndcX, ndcY, 1.0f, 1.0f); // far plane

	// 3. 逆射影行列でview空間に戻す
	Matrix4x4 invProj = Matrix4x4::Inverse(proj);
	Vector4 nearView = invProj * nearPoint;
	Vector4 farView = invProj * farPoint;

	// 4. 視点空間からワールド空間へ変換
	Matrix4x4 invView = Matrix4x4::Inverse(view);
	Vector4 nearWorld = invView * (nearView / nearView.w);
	Vector4 farWorld = invView * (farView / farView.w);

	// 5. レイ作成
	Vector3 origin = nearWorld.xyz();
	Vector3 direction = (farWorld.xyz() - origin).Normalize();

	return Ray {origin, direction};
}
