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

std::optional<RaycastHit> Raycastor::Raycast(const Ray& ray, const std::vector<std::shared_ptr<SceneObject>>& objects, float maxDistance) {
	std::optional<RaycastHit> closestHit;

	for (const auto& obj : objects){
		if (!obj) continue;
		if (!obj->IsEnableRaycast()) continue;

		AABB box = obj->GetWorldAABB();
		float t = maxDistance;

		if (IntersectRayAABB(ray, box, t) && t < maxDistance){
			closestHit = RaycastHit {
				.distance = t,
				.point = ray.origin + ray.direction * t,
				.normal = Vector3(),
				.hitObject = obj  //<-- shared_ptr のまま渡す
			};
			maxDistance = t;
		}
	}
	return closestHit;
}

Ray Raycastor::ConvertMouseToRay(const Vector2& mousePos, const Matrix4x4& viewMatrix, const Matrix4x4& projMatrix, const Vector2& viewportSize) {
	float x = (2.0f * mousePos.x) / viewportSize.x - 1.0f;
	float y = 1.0f - (2.0f * mousePos.y) / viewportSize.y; // Y軸反転に注意
	Vector3 rayNDC = { x, y, 1.0f }; // Z=1: 遠平面

	// 2. NDC → View 空間に逆変換（逆射影）
	Matrix4x4 invProj = Matrix4x4::Inverse(projMatrix);
	Vector4 rayView = invProj * Vector4(rayNDC, 1.0f);
	rayView /= rayView.w;
	rayView.z = 1.0f; rayView.w = 0.0f; // 視線方向のベクトルに変換

	// 3. View → ワールド空間に変換
	Matrix4x4 invView = Matrix4x4::Inverse(viewMatrix);
	Vector3 rayDirWorld = (invView * rayView).xyz();
	rayDirWorld = rayDirWorld.Normalize();

	Vector3 rayOrigin =  invView.GetTranslationMatrix();

	return Ray{ rayOrigin, rayDirWorld };
}