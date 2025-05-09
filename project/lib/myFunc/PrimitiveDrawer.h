#pragma once

#include <Engine/objects/Mesh/Primitive/Drawer/LineDrawer.h>
#include <memory>
#include <vector>

struct Vector3;
struct Vector4;
struct Matrix4x4;

class PrimitiveDrawer{
public:
	static PrimitiveDrawer* GetInstance();
	~PrimitiveDrawer() = default;

	void Initialize();
	void Finalize();
	void DrawLine3d(const Vector3& start, const Vector3& end, const Vector4& color);
	void Update(); // 寿命付き対応時など
	void Render();
	void ClearMesh();

	void DrawGrid();
	void DrawOBB(const Vector3& center, const Vector3& rotate, const Vector3& size, const Vector4 color);
	void DrawSphere(const Vector3& center, const float radius, int subdivision, Vector4 color);

private:
	PrimitiveDrawer() = default;

private:
	std::unique_ptr<LineDrawer> lineDrawer_;
};
