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
	// 1. スクリーン空間 → 正規化デバイス座標（NDC）
	float ndcX = (2.0f * mousePos.x) / viewportSize.x - 1.0f;
	float ndcY = 1.0f - (2.0f * mousePos.y) / viewportSize.y; // Y軸反転
	float ndcZ = 1.0f; // DirectXは Z=1 が far plane

	Vector3 ndcRay = Vector3(ndcX, ndcY, ndcZ);

	// 2. NDC → View space（視錐台座標系）
	Matrix4x4 invProj = Matrix4x4::Inverse(proj);
	Vector4 rayView = invProj * Vector4(ndcRay, 1.0f);
	rayView.z = 1.0f; // ディレクションベクトルにするため
	rayView.w = 0.0f;

	// 3. View space → World space
	Matrix4x4 invView = Matrix4x4::Inverse(view);
	Vector4 rayWorld = invView * rayView;

	Vector3 rayDir = rayWorld.xyz().Normalize();
	Vector3 rayOrigin = Matrix4x4::Translation(invView); // カメラ位置

	return Ray {rayOrigin, rayDir};
}
