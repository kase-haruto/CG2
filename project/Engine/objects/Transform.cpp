#include "Transform.h"

#include <Engine/graphics/GraphicsGroup.h>
#include <lib/myFunc/MyFunc.h>
#include <externals/imgui/imgui.h>


void EulerTransform::ShowImGui(const std::string& lavel){
    if (ImGui::CollapsingHeader(lavel.c_str())){
        ImGui::DragFloat3("scale", &scale.x, 0.01f);
        ImGui::DragFloat3("rotation", &rotate.x, 0.01f);
        ImGui::DragFloat3("translate", &translate.x, 0.01f);
    }
}

/* ========================================================================
/* baseTransform class
/* ===================================================================== */
void BaseTransform::Initialize(){

	// デフォルト値
	scale.Initialize({1.0f, 1.0f, 1.0f});
	rotation.Initialize();
	translation.Initialize();

    //バッファの作成
    DxConstantBuffer::Initialize(GraphicsGroup::GetInstance()->GetDevice());
}

Vector3 BaseTransform::GetWorldPosition() const{
	Vector3 worldPos {};
	worldPos.x = matrix.world.m[3][0];
	worldPos.y = matrix.world.m[3][1];
	worldPos.z = matrix.world.m[3][2];

	return worldPos;
}

/* ========================================================================
/* worldTransform class
/* ===================================================================== */
void WorldTransform::Update(const Matrix4x4& viewProMatrix){
	Matrix4x4 worldMatrix =
		MakeAffineMatrix(scale, rotation, translation);
	if (parent){

		worldMatrix = Matrix4x4::Multiply(worldMatrix, parent->matrix.world);
	}
	Matrix4x4 wvpMatrix = worldMatrix * viewProMatrix;
	Matrix4x4 worldInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(worldMatrix));

	matrix.world = worldMatrix;
	matrix.WVP = wvpMatrix;
	matrix.WorldInverseTranspose = worldInverseTranspose;

	DxConstantBuffer::TransferData(matrix);
}
