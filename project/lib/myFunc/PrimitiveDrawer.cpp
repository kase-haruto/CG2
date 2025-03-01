#include "lib/myFunc/PrimitiveDrawer.h"
#include "engine/graphics/GraphicsGroup.h"
#include "Engine/graphics/camera/CameraManager.h"
#include "lib/myFunc/MyFunc.h"


PrimitiveDrawer::PrimitiveDrawer(){
	// GraphicsGroup から必要なデバイスとコマンドリストを取得
	device_ = GraphicsGroup::GetInstance()->GetDevice();
	commandList_ = GraphicsGroup::GetInstance()->GetCommandList();
	rootSignature_ = GraphicsGroup::GetInstance()->GetRootSignature(Line);
	pipelineState_ = GraphicsGroup::GetInstance()->GetPipelineState(Line);
}

PrimitiveDrawer::~PrimitiveDrawer(){}

void PrimitiveDrawer::Initialize(){
	// 描画用のメッシュを作成
	CreateMeshes();
	CreateMatrixBuffer();
}

void PrimitiveDrawer::Finalize(){
	line_->indexBuff.Reset();
	line_->vertBuff.Reset();
	wvpResource_.Reset();

	device_.Reset();
	commandList_.Reset();
	pipelineState_.Reset();
	rootSignature_.Reset();

}

void PrimitiveDrawer::CreateMeshes(){
	// 最大の線の数を想定してメッシュを作成
	const UINT maxVertices = kMaxLineCount * kVertexCountLine; // ラインは2頂点で構成
	const UINT maxIndices = 20; // インデックスバッファは使わない場合

	// ライン描画用のメッシュを作成
	line_ = CreateMesh(maxVertices, maxIndices);
}

std::unique_ptr<PrimitiveDrawer::Mesh> PrimitiveDrawer::CreateMesh(UINT vertexCount, UINT indexCount){
	auto mesh = std::make_unique<Mesh>();

	// 頂点バッファのサイズを設定
	UINT vertexBufferSize = sizeof(VertexPosColor) * vertexCount;

	// 頂点バッファのリソースを作成
	mesh->vertBuff = CreateBufferResource(device_, vertexBufferSize);

	// 頂点バッファのビューを設定
	mesh->vbView.BufferLocation = mesh->vertBuff->GetGPUVirtualAddress();
	mesh->vbView.StrideInBytes = sizeof(VertexPosColor);
	mesh->vbView.SizeInBytes = vertexBufferSize;

	// 頂点バッファをマップ
	D3D12_RANGE readRange = {0, 0}; // CPUによる読み取りは行わない
	mesh->vertBuff->Map(0, &readRange, reinterpret_cast< void** >(&mesh->vertMap));

	// インデックスバッファのサイズを設定
	UINT indexBufferSize = sizeof(uint16_t) * indexCount;

	// インデックスバッファのリソースを作成
	if (indexCount > 0){
		mesh->indexBuff = CreateBufferResource(device_, indexBufferSize);

		// インデックスバッファのビューを設定
		mesh->ibView.BufferLocation = mesh->indexBuff->GetGPUVirtualAddress();
		mesh->ibView.Format = DXGI_FORMAT_R16_UINT;
		mesh->ibView.SizeInBytes = indexBufferSize;

		// インデックスバッファをマップ
		mesh->indexBuff->Map(0, &readRange, reinterpret_cast< void** >(&mesh->indexMap));
	}

	return mesh;
}

void PrimitiveDrawer::DrawLine3d(const Vector3& p1, const Vector3& p2, const Vector4& color){
	// 使用可能な最大線分数を超えていないか確認
	if (indexLine_ < kMaxLineCount){
		// ラインの頂点データをバッファに追加
		line_->vertMap[indexLine_ * 2] = {p1, color};
		line_->vertMap[indexLine_ * 2 + 1] = {p2, color};

		// 次のインデックスに進む
		indexLine_++;
	}
}

void PrimitiveDrawer::DrawOBB(const Vector3& center, const Vector3& rotate, const Vector3& size, const Vector4 color){

	const uint32_t vertexNum = 8;

	// 回転行列を計算
	Matrix4x4 rotationMatrix = EulerToMatrix(rotate);

	// 各軸の半サイズを回転
	Vector3 halfSizeX = Vector3::Transform({1.0f, 0.0f, 0.0f}, rotationMatrix) * size.x * 0.5f;
	Vector3 halfSizeY = Vector3::Transform({0.0f, 1.0f, 0.0f}, rotationMatrix) * size.y * 0.5f;
	Vector3 halfSizeZ = Vector3::Transform({0.0f, 0.0f, 1.0f}, rotationMatrix) * size.z * 0.5f;

	// 頂点を計算
	Vector3 vertices[vertexNum];
	Vector3 offsets[vertexNum] = {
		{-1, -1, -1}, {-1,  1, -1}, {1, -1, -1}, {1,  1, -1},
		{-1, -1,  1}, {-1,  1,  1}, {1, -1,  1}, {1,  1,  1}
	};

	for (int i = 0; i < vertexNum; ++i){
		Vector3 localVertex = offsets[i].x * halfSizeX +
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
		int start = edges[i][0];
		int end = edges[i][1];
		DrawLine3d(vertices[start], vertices[end], color);
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
	if (indexLine_ == 0) return; // 描画する線がない場合は終了

	UpdateMatrixBuffer();

	GraphicsGroup::GetInstance()->SetCommand(commandList_,PipelineType::Line,BlendMode::NORMAL);

	// プリミティブトポロジをラインリストに設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	// 頂点バッファを設定
	D3D12_VERTEX_BUFFER_VIEW vbView = line_->vbView;
	commandList_->IASetVertexBuffers(0, 1, &vbView);
	// wvp用のCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, wvpResource_->GetGPUVirtualAddress());

	// ラインを描画
	commandList_->DrawInstanced(indexLine_ * 2, 1, 0, 0);

	//indexLine_をリセット
	indexLine_ = 0;
}

void PrimitiveDrawer::CreateMatrixBuffer(){
	wvpResource_ = CreateBufferResource(device_, sizeof(TransformationMatrix));
	wvpResource_->Map(0, nullptr, reinterpret_cast< void** >(&matrixData_));
}

void PrimitiveDrawer::UpdateMatrixBuffer(){
	// ワールド行列（必要に応じて設定）
	Matrix4x4 worldMatrix = Matrix4x4::MakeIdentity();
	Matrix4x4 worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, CameraManager::GetViewProjectionMatrix());

	// 定数バッファの更新
	matrixData_->world = worldMatrix;
	matrixData_->WVP = worldViewProjectionMatrix;
}