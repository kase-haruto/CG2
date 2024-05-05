#include "FogEffect.h"

FogEffect::FogEffect(int32_t width,
					 int32_t height,
					 ID3D12Device* device,
					 ID3D12GraphicsCommandList* commandList
){
	//定数バッファの作成
	bufferSize = sizeof(FogParameters);

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constantBuffer));

	//定数バッファをマップ
	constantBuffer.Get()->Map(0, nullptr, reinterpret_cast< void** >(&mappedConstantBuffer));

	//デフォルトの霧のパラメータを設定
	parameters.fogStart = 10.0f;
	parameters.fogEnd = 100.0f;
	parameters.fogColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	//ピクセルシェーダに霧の定数バッファをバインド
	commandList->SetGraphicsRootConstantBufferView(3, constantBuffer->GetGPUVirtualAddress());
}

FogEffect::~FogEffect(){
	if (constantBuffer != nullptr){
		constantBuffer.Get()->Unmap(0, nullptr);
	}
}

void FogEffect::Update(){
	memcpy(mappedConstantBuffer, &parameters, sizeof(FogParameters));
}

void FogEffect::Apply(ID3D12GraphicsCommandList* commandList, ID3D12Device* device){
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
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	// 他の設定（頂点シェーダー、ピクセルシェーダーなど）も行う
	psoDesc.BlendState = blendDesc;

	// グラフィックスパイプラインステートオブジェクトを作成
	ID3D12PipelineState* pipelineStateObject;
	device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineStateObject));

	// コマンドリストにグラフィックスパイプラインステートを設定
	commandList->SetPipelineState(pipelineStateObject);

	// グラフィックスパイプラインステートオブジェクトの解放
	pipelineStateObject->Release();
}