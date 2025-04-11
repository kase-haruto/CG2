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
/////////////////////////////////////////////////////////////////////////////////////////
//	初期化処理
/////////////////////////////////////////////////////////////////////////////////////////
void BaseTransform::Initialize(){
	// デフォルト値
	scale.Initialize({1.0f, 1.0f, 1.0f});
	rotation.Initialize();
	translation.Initialize();

	//バッファの作成
	DxConstantBuffer::Initialize(GraphicsGroup::GetInstance()->GetDevice());
}



/////////////////////////////////////////////////////////////////////////////////////////
//	imgui
/////////////////////////////////////////////////////////////////////////////////////////
void BaseTransform::ShowImGui(const std::string& lavel){
	if (ImGui::CollapsingHeader(lavel.c_str())){
		ImGui::DragFloat3("scale", &scale.x, 0.01f);
		ImGui::DragFloat3("rotation", &eulerRotation.x, 0.01f);
		ImGui::DragFloat3("translate", &translation.x, 0.01f);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//	ワールド座標空間での位置を取得
/////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////
//	worldTransformの更新
/////////////////////////////////////////////////////////////////////////////////////////
void WorldTransform::Update(const Matrix4x4& viewProMatrix){
	// eulerRotationをQuaternionに変換
	rotation = Quaternion::EulerToQuaternion(eulerRotation);

	Matrix4x4 scaleMat = MakeScaleMatrix(scale);
	Matrix4x4 rotateMat = Quaternion::ToMatrix(rotation);
	Matrix4x4 translateMat = MakeTranslateMatrix(translation);

	Matrix4x4 localMat = scaleMat * rotateMat * translateMat;

	// 親がいれば合成
	if (parent){
		matrix.world = localMat * parent->matrix.world;
	} else{
		matrix.world = localMat;
	}

	matrix.WVP = matrix.world * viewProMatrix;
	matrix.WorldInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(matrix.world));

	// 定数バッファに反映
	TransferData(matrix);
}
