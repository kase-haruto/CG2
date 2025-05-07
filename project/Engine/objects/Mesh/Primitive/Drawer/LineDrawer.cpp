#include "LineDrawer.h"

#include <lib/myMath/Vector3.h>
#include <lib/myMath/Vector4.h>
#include <lib/myMath/Matrix4x4.h>
#include <Engine/graphics/GraphicsGroup.h>
#include <Engine/graphics/camera/CameraManager.h>

void LineDrawer::Initialize(){
	vertexBuffer_.Initialize(GraphicsGroup::GetInstance()->GetDevice(), kMaxLines * 2);
	wvpBuffer_.Initialize(GraphicsGroup::GetInstance()->GetDevice(), 1);
}

void LineDrawer::DrawLine(const Vector3& start, const Vector3& end, const Vector4& color){
	if (vertices_.size() / 2 >= kMaxLines) return;

	vertices_.emplace_back(VertexPosColor {start, color});
	vertices_.emplace_back(VertexPosColor {end, color});
}

void LineDrawer::Render(const Matrix4x4& vp){
	if (vertices_.empty()) return;

	auto cmdList = GraphicsGroup::GetInstance()->GetCommandList();
	GraphicsGroup::GetInstance()->SetCommand(cmdList, PipelineType::Line, BlendMode::NORMAL);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	// 頂点バッファ更新
	vertexBuffer_.TransferVectorData(vertices_);
	vertexBuffer_.SetCommand(cmdList);

	// WVP定数バッファ更新
	Matrix4x4 identity = Matrix4x4::MakeIdentity();

	TransformationMatrix wvpData;
	wvpData.world = identity;
	wvpData.WVP = identity * vp;
	wvpData.WorldInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(identity));

	wvpBuffer_.TransferData(wvpData);
	wvpBuffer_.SetCommand(cmdList, 0); // RootParam 0

	// 描画
	cmdList->DrawInstanced(static_cast< UINT >(vertices_.size()), 1, 0, 0);

	vertices_.clear();
}


void LineDrawer::Clear(){
	vertices_.clear();
}
