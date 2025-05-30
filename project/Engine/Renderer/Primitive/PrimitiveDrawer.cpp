#include "PrimitiveDrawer.h"

#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Foundation/Math/Vector4.h>
#include <Engine/Foundation/Math/Matrix4x4.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>

#include <Engine/Graphics/Context/GraphicsGroup.h>

#include <cmath>
#include <numbers>

PrimitiveDrawer* PrimitiveDrawer::GetInstance(){
	static PrimitiveDrawer instance;
	return &instance;
}

void PrimitiveDrawer::Initialize(){
	lineDrawer_ = std::make_unique<LineDrawer>();
	lineDrawer_->Initialize();

	boxDrawer_ = std::make_unique<BoxDrawer>();
	boxDrawer_->Initialize();
}

void PrimitiveDrawer::Finalize(){
	if (lineDrawer_){
		lineDrawer_->Clear();
	}
	lineDrawer_.reset();

	if (boxDrawer_) {
		boxDrawer_->Clear();
	}
	boxDrawer_.reset();
}


void PrimitiveDrawer::DrawLine3d(const Vector3& start, const Vector3& end, const Vector4& color){
	if (lineDrawer_){
		lineDrawer_->DrawLine(start, end, color);
	}
}

void PrimitiveDrawer::DrawBox(const Vector3& center, Quaternion& rotate, const Vector3& size, const Vector4& color) {
	if (boxDrawer_) {
		boxDrawer_->DrawBox(center, rotate,size, color);
	}
}

void PrimitiveDrawer::DrawGrid(){
	const uint32_t kSubdivision = 32;
	const float kGridHalfWidth = 32.0f;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);

	for (uint32_t index = 0; index <= kSubdivision; ++index){
		float offset = -kGridHalfWidth + index * kGridEvery;

		// --- 縦線（Z軸方向） ---
		Vector3 verticalStart(offset, 0.0f, kGridHalfWidth);
		Vector3 verticalEnd(offset, 0.0f, -kGridHalfWidth);

		Vector4 verticalColor = (std::abs(offset) < 0.001f) ? Vector4(0.0f, 1.0f, 0.0f, 1.0f) // X=0 line
			: Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		DrawLine3d(verticalStart, verticalEnd, verticalColor);

		Vector3 horizontalStart(-kGridHalfWidth, 0.0f, offset);
		Vector3 horizontalEnd(kGridHalfWidth, 0.0f, offset);

		Vector4 horizontalColor = (std::abs(offset) < 0.001f) ? Vector4(1.0f, 0.0f, 0.0f, 1.0f) // Z=0 line
			: Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		DrawLine3d(horizontalStart, horizontalEnd, horizontalColor);
	}
}

void PrimitiveDrawer::DrawOBB(const Vector3& center, const Quaternion& rotate, const Vector3& size, const Vector4& color){
	const uint32_t vertexNum = 8;

	// 各軸の半サイズをクォータニオンで回転
	Vector3 halfSizeX = Vector3::Transform({1.0f, 0.0f, 0.0f}, rotate) * (size.x * 0.5f);
	Vector3 halfSizeY = Vector3::Transform({0.0f, 1.0f, 0.0f}, rotate) * (size.y * 0.5f);
	Vector3 halfSizeZ = Vector3::Transform({0.0f, 0.0f, 1.0f}, rotate) * (size.z * 0.5f);

	// 頂点を計算
	Vector3 vertices[vertexNum];
	Vector3 offsets[vertexNum] = {
		{-1, -1, -1}, {-1,  1, -1}, {1, -1, -1}, {1,  1, -1},
		{-1, -1,  1}, {-1,  1,  1}, {1, -1,  1}, {1,  1,  1}
	};

	for (int i = 0; i < vertexNum; ++i){
		Vector3 localVertex =
			offsets[i].x * halfSizeX +
			offsets[i].y * halfSizeY +
			offsets[i].z * halfSizeZ;
		vertices[i] = center + localVertex;
	}

	// 辺を描画
	int edges[12][2] = {
		{0, 1}, {1, 3}, {3, 2}, {2, 0},
		{4, 5}, {5, 7}, {7, 6}, {6, 4},
		{0, 4}, {1, 5}, {2, 6}, {3, 7}
	};

	for (int i = 0; i < 12; ++i){
		DrawLine3d(vertices[edges[i][0]], vertices[edges[i][1]], color);
	}
}

void PrimitiveDrawer::DrawSphere(const Vector3& center, const float radius, int subdivision, Vector4 color){

	// 分割数
	const uint32_t kSubdivision = subdivision; // 分割数を増やして滑らかに
	const float kLonEvery = 2 * float(std::numbers::pi) / kSubdivision;
	const float kLatEvery = float(std::numbers::pi) / kSubdivision;
	Vector3 a, b, c, d;

	// 緯度方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex){
		float lat = -float(std::numbers::pi) / 2.0f + kLatEvery * latIndex;
		float nextLat = lat + kLatEvery;

		// 経度方向に分割
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex){
			float lon = lonIndex * kLonEvery;
			float nextLon = lon + kLonEvery;

			// 点の座標を計算
			a.x = radius * (std::cos(lat) * std::cos(lon)) + center.x;
			a.y = radius * std::sin(lat) + center.y;
			a.z = radius * (std::cos(lat) * std::sin(lon)) + center.z;

			b.x = radius * (std::cos(nextLat) * std::cos(lon)) + center.x;
			b.y = radius * std::sin(nextLat) + center.y;
			b.z = radius * (std::cos(nextLat) * std::sin(lon)) + center.z;

			c.x = radius * (std::cos(lat) * std::cos(nextLon)) + center.x;
			c.y = radius * std::sin(lat) + center.y;
			c.z = radius * (std::cos(lat) * std::sin(nextLon)) + center.z;

			d.x = radius * (std::cos(nextLat) * std::cos(nextLon)) + center.x;
			d.y = radius * std::sin(nextLat) + center.y;
			d.z = radius * (std::cos(nextLat) * std::sin(nextLon)) + center.z;

			// 経度方向の線を描画
			DrawLine3d(a, c, color);
			DrawLine3d(a, b, color);

			// 緯度方向の線を描画
			DrawLine3d(b, d, color);
			DrawLine3d(c, d, color);
		}
	}

}

void PrimitiveDrawer::Render(){
	if (lineDrawer_){
		lineDrawer_->Render();
	}

	if (boxDrawer_) {
		boxDrawer_->Render();
	}
}

void PrimitiveDrawer::ClearMesh(){
	if (lineDrawer_){
		lineDrawer_->Clear();
	}

	if (boxDrawer_) {
		boxDrawer_->Clear();
	}
}
