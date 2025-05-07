#include "LineMesh.h"
#include "lib/myMath/Vector3.h"
#include <lib/myMath/Matrix4x4.h>
#include "lib/myMath/Quaternion.h"
#include <Engine/graphics/GraphicsGroup.h>

LineMesh::LineMesh(const Vector3& start, const Vector3& end)
	: start_(start), end_(end){
	if (!sharedInitialized_){
		InitializeSharedBuffer();
		sharedInitialized_ = true;
	}

	transform_.Initialize();

	// 始点 → 終点ベクトル
	Vector3 dir = end - start;
	float length = dir.Length();

	// スケールと位置
	transform_.scale = {length, 1.0f, 1.0f};
	transform_.translation = start;

	// X軸ベクトルに向ける回転
	Vector3 xAxis = {1.0f, 0.0f, 0.0f};
	Vector3 normalizedDir = dir.Normalize();

	transform_.rotation = Quaternion::FromToQuaternion(xAxis, normalizedDir);
}

void LineMesh::InitializeSharedBuffer(){
	auto device = GraphicsGroup::GetInstance()->GetDevice();

	std::vector<VertexPosColor> vertices = {
		{ {0, 0, 0}, {1, 1, 1, 1} },
		{ {1, 0, 0}, {1, 1, 1, 1} }
	};

	sharedVertexBuffer_.Initialize(device, static_cast< UINT >(vertices.size()));
	sharedVertexBuffer_.TransferVectorData(vertices);
}

void LineMesh::Draw(const Matrix4x4& vp){
	auto cmdList = GraphicsGroup::GetInstance()->GetCommandList();

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	sharedVertexBuffer_.SetCommand(cmdList);

	transform_.Update(vp);
	transform_.SetCommand(cmdList, 0);
	cmdList->DrawInstanced(2, 1, 0, 0);
}

void LineMesh::SetColor(const Vector4& color){
	color_ = color;

	std::vector<VertexPosColor> vertices = {
		{ {0, 0, 0}, color },
		{ {1, 0, 0}, color }
	};
	sharedVertexBuffer_.TransferVectorData(vertices);
}

const Matrix4x4& LineMesh::GetWorldMatrix() const{
	return transform_.matrix.world;
}

void LineMesh::SetBlendMode(BlendMode mode){
	blendMode_ = mode;
}

BlendMode LineMesh::GetBlendMode() const{
	return blendMode_;
}
