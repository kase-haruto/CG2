// BaseCamera.cpp
#include "Engine/graphics/camera/BaseCamera.h"
#include "Engine/graphics/GraphicsGroup.h"
#include "lib/myFunc/MyFunc.h"
#include <cmath>
#include "BaseCamera.h"

BaseCamera::BaseCamera()
	: ICamera(),
	viewMatrix_(Matrix4x4::Inverse(worldMatrix_)),
	projectionMatrix_(MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_)){
	viewProjectionMatrix_ = Matrix4x4::Multiply(viewMatrix_, projectionMatrix_);

	/* バッファの生成とマッピング =======================*/
	CreateBuffer();
	Map();
}

/////////////////////////////////////////////////////////////////////////
//  更新
/////////////////////////////////////////////////////////////////////////
void BaseCamera::Update(){
	if (!isActive_){ return; }//アクティブでない場合は処理しない

	UpdateMatrix();

	ComPtr<ID3D12GraphicsCommandList> commandList = GraphicsGroup::GetInstance()->GetCommandList();
	ComPtr<ID3D12RootSignature> rootSignature = GraphicsGroup::GetInstance()->GetRootSignature(Object3D);

	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetGraphicsRootConstantBufferView(5, constBuffer_->GetGPUVirtualAddress());

}

/////////////////////////////////////////////////////////////////////////
//  行列の更新
/////////////////////////////////////////////////////////////////////////
void BaseCamera::UpdateMatrix(){
	// 行列の更新
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Matrix4x4::Inverse(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_);
	viewProjectionMatrix_ = Matrix4x4::Multiply(viewMatrix_, projectionMatrix_);
}


/////////////////////////////////////////////////////////////////////////
//  projection行列の作成
/////////////////////////////////////////////////////////////////////////
Matrix4x4 BaseCamera::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip){
	Matrix4x4 result = {
		1 / (aspectRatio * std::tan(fovY / 2)), 0, 0, 0,
		0, 1 / std::tan(fovY / 2), 0, 0,
		0, 0, farClip / (farClip - nearClip), 1,
		0, 0, -nearClip * farClip / (farClip - nearClip), 0
	};
	return result;
}

void BaseCamera::SetName(const std::string& name){
	SceneObject::SetName(name, ObjectType::Camera);
}

/////////////////////////////////////////////////////////////////////////
//  アクセッサ
/////////////////////////////////////////////////////////////////////////
#pragma region アクセッサ
void BaseCamera::SetCamera(const Vector3& pos, const Vector3& rotate){
	transform_.translate = pos;
	transform_.rotate = rotate;
}




const Matrix4x4& BaseCamera::GetViewMatrix() const{
	return viewMatrix_;
}

const Matrix4x4& BaseCamera::GetProjectionMatrix() const{
	return projectionMatrix_;
}

const Matrix4x4& BaseCamera::GetViewProjectionMatrix() const{
	return viewProjectionMatrix_;
}

const Vector3& BaseCamera::GetRotate() const{
	return transform_.rotate;
}

const Vector3& BaseCamera::GetTranslate() const{
	return transform_.translate;
}
#pragma endregion

/////////////////////////////////////////////////////////////////////////
//  バッファの生成とマッピング
/////////////////////////////////////////////////////////////////////////
#pragma region バッファの生成とマッピング
void BaseCamera::CreateBuffer(){
	ComPtr<ID3D12Device> device = GraphicsGroup::GetInstance()->GetDevice();
	constBuffer_ = CreateBufferResource(device.Get(), sizeof(Camera3dForGPU));
}

void BaseCamera::Map(){
	constBuffer_->Map(0, nullptr, reinterpret_cast< void** >(&cameraData_));
	cameraData_->worldPosition = transform_.translate;
	constBuffer_->Unmap(0, nullptr);  // データ設定後にアンマップ
}

#pragma endregion