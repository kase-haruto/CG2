#include "FogEffect.h"
#include"DirectXCommon.h"

FogEffect::~FogEffect(){
	device_->Release();
	commandList_->Release();
	if (constantBuffer != nullptr){
		constantBuffer.Get()->Unmap(0, nullptr);
	}
}

FogEffect::FogEffect(DirectXCommon* dxCommon){

	dxCommon_ = dxCommon;
	device_ = dxCommon->GetDevice();
	commandList_ = dxCommon->GetCommandList();

	//定数バッファの生成
	CreateConstantBuffer();

	///================================
	///	霧のパラメータを設定
	///================================
	//霧のスタート地点
	parameters.fogStart = 10.0f;
	//霧の終点
	parameters.fogEnd = 100.0f;
	//霧の色
	parameters.fogColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);

	//定数バッファをマップ
	constantBuffer->Map(0, nullptr, reinterpret_cast< void** >(&parameters));
}

void FogEffect::CreateConstantBuffer(){
	//定数バッファの作成
	bufferSize = sizeof(FogParameters);

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = bufferSize; // 定数バッファのサイズを設定
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
		IID_PPV_ARGS(&constantBuffer));
}



void FogEffect::Update(){
	memcpy(mappedConstantBuffer, &parameters, sizeof(FogParameters));
}

