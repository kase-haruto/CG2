#include "ViewProjection.h"
#include<cmath>
#include"DirectXCommon.h"
#include<imgui.h>
#include"MyFunc.h"

ViewProjection::ViewProjection(DirectXCommon* dxCommon){
	dxCommon_ = dxCommon;
	commandList_ = dxCommon->GetCommandList();
	device_ = dxCommon->GetDevice();

	//定数バッファの生成
	CreateConstBuffer();

	Initialize();

	//マップ
	constBuffer_->Map(0, nullptr, reinterpret_cast< void** >(&camearData_));

	//カメラのパラメータ
	camearData_->view = matView;
	camearData_->projection = matProjection;
	camearData_->cameraPos = transform.translate;
}

ViewProjection::~ViewProjection(){
	commandList_->Release();
	device_->Release();
}

void ViewProjection::CreateConstBuffer(){
	//定数バッファの作成
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = sizeof(ConstBufferDataViewProjection); // 定数バッファのサイズを設定
	desc.Height = 1; // バッファの高さは1
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN; // 定数バッファなのでフォーマットは不明
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; // 定数バッファなので行主要レイアウト
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;


	// リソースの作成
	HRESULT hr = device_->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // 一般的な読み取り状態で初期化
		nullptr,
		IID_PPV_ARGS(&constBuffer_));
}

void ViewProjection::Map(){

}

void ViewProjection::Initialize(){
	Matrix4x4 cameraMatrix = MakeAffineMatrix(transform.scale,
														 transform.rotate,
														 transform.translate
	);
	matView = Matrix4x4::Inverse(cameraMatrix);
	matProjection = MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
	viewProjection_ = Matrix4x4::Multiply(matView, matProjection);
}

void ViewProjection::UpdateMatrix(){
	UpdateViewMatrix();
}

void ViewProjection::UpdateViewMatrix(){
	Matrix4x4 cameraMatrix = MakeAffineMatrix(transform.scale,
														 transform.rotate,
														 transform.translate
	);
	matView = Matrix4x4::Inverse(cameraMatrix);
}

void ViewProjection::UpdateProjectionMatrix(){
	matProjection = MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
}

Matrix4x4 ViewProjection::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip){
	Matrix4x4 result;
	result = {
		1 / aspectRatio * 1 / std::tan(fovY / 2),0,0,0,
		0,1 / std::tan(fovY / 2),0,0,
		0,0,farClip / (farClip - nearClip),1,
		0,0,-nearClip * farClip / (farClip - nearClip),0
	};
	return result;
}

void ViewProjection::ImGui(){
	UpdateMatrix();
	ImGui::Begin("camera");
	ImGui::DragFloat3("pos", &transform.translate.x, 0.01f);
	ImGui::End();
}