#include "Transform.h"
/* ========================================================================
/* include space
/* ===================================================================== */

// engine
#include <Engine/Graphics/Context/GraphicsGroup.h>

// lib
#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <externals/imgui/imgui.h>


void EulerTransform::ShowImGui(const std::string& label){
	ImGui::SeparatorText(label.c_str());
	std::string scaleLabel = label + "_scale";
	std::string rotationLabel = label + "_rotation";
	std::string translationLabel = label + "_translate";
	ImGui::DragFloat3(scaleLabel.c_str(), &scale.x, 0.01f);
	ImGui::DragFloat3(rotationLabel.c_str(), &rotate.x, 0.01f);
	ImGui::DragFloat3(translationLabel.c_str(), &translate.x, 0.01f);
}

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
void BaseTransform::ShowImGui(const std::string& label){
	ImGui::SeparatorText(label.c_str());
	std::string scaleLabel = label + "_scale";
	std::string rotationLabel = label + "_rotation";
	std::string translationLabel = label + "_translate";
	ImGui::DragFloat3(scaleLabel.c_str(), &scale.x, 0.01f);
	ImGui::DragFloat3(rotationLabel.c_str(), &eulerRotation.x, 0.01f);
	ImGui::DragFloat3(translationLabel.c_str(), &translation.x, 0.01f);
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

	Matrix4x4 scaleMat = MakeScaleMatrix(scale);
	if (eulerRotation != Vector3 {0.0f, 0.0f, 0.0f}){
		rotation = Quaternion::EulerToQuaternion(eulerRotation);
	}
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

void WorldTransform::Update(){
	Matrix4x4 scaleMat = MakeScaleMatrix(scale);
	if (eulerRotation != Vector3 {0.0f, 0.0f, 0.0f}){
		rotation = Quaternion::EulerToQuaternion(eulerRotation);
	}
	Matrix4x4 rotateMat = Quaternion::ToMatrix(rotation);
	Matrix4x4 translateMat = MakeTranslateMatrix(translation);

	Matrix4x4 localMat = scaleMat * rotateMat * translateMat;

	// 親がいれば合成
	if (parent){
		matrix.world = localMat * parent->matrix.world;
	} else{
		matrix.world = localMat;
	}
	matrix.WorldInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(matrix.world));

	TransferData(matrix);
}

void Transform2D::ShowImGui(const std::string& lavel){
	if (ImGui::CollapsingHeader(lavel.c_str())){
		ImGui::DragFloat2("scale", &scale.x, 0.01f);
		ImGui::DragFloat("rotation", &rotate, 0.01f);
		ImGui::DragFloat2("translate", &translate.x, 0.01f);
	}
}
