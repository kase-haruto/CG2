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
	//定数バッファをマップ
	constantBuffer.Get()->Map(0, nullptr, reinterpret_cast< void** >(&parameters));


	///================================
	///	霧のパラメータを設定
	///================================
	//霧のスタート地点
	parameters.fogStart = 10.0f;
	//霧の終点
	parameters.fogEnd = 100.0f;
	//霧の色
	parameters.fogColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);


	//ピクセルシェーダに霧の定数バッファをバインド
	commandList_->SetGraphicsRootConstantBufferView(2, constantBuffer->GetGPUVirtualAddress());
}

void FogEffect::CreateConstantBuffer(){
	//定数バッファの作成
	bufferSize = sizeof(FogParameters);

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = sizeof(FogParameters); // 定数バッファのサイズを設定
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

void FogEffect::Apply(){
	// ブレンドステートの設定
	D3D12_BLEND_DESC blendDesc {};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	// ブレンドステートを含むグラフィックスパイプラインステートオブジェクトの作成
	

	// グラフィックスパイプラインステートオブジェクトを作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = dxCommon_->GetGraphicsPSODesc();
	ID3D12PipelineState* pipelineStateObject;
	device_->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineStateObject));

	// コマンドリストにグラフィックスパイプラインステートを設定
	commandList_->SetPipelineState(pipelineStateObject);
}


void FogEffect::Update(){
	memcpy(mappedConstantBuffer, &parameters, sizeof(FogParameters));
}

