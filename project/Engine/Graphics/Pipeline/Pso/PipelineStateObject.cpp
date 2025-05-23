#include "PipelineStateObject.h"
#include <Engine/Foundation/Utility/ConvertString/ConvertString.h>

/* c++ */
#include<cassert>

void PipelineStateObject::CreateRootSignature(ID3D12Device* device) {
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//シリアライズしてバイナリする
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
											   D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリをもとに生成
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
												signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
}

void PipelineStateObject::BindInputLayout() {
	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[1] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = _countof(inputElementDesc);
}

void PipelineStateObject::BindBlendState() {
	//BlendStateの設定
	
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
}

void PipelineStateObject::BintRasterizerState() {
	//RasiterzerStateの設定
	
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
}

void PipelineStateObject::CompileShader(ShaderCompiler* sc) {
	//shaderをコンパイルする
	vertexShaderBlob = sc->CompileShader(L"Object3D.Vs.hlsl", L"vs_6_0", sc->GetDxcCompiler());
	assert(vertexShaderBlob != nullptr);

	pixelShaderBlob = sc->CompileShader(L"Object3D.PS.hlsl", L"vs_6_0", sc->GetDxcCompiler());
	assert(vertexShaderBlob != nullptr);
}

void PipelineStateObject::CreatePSO(ID3D12Device* device) {
	HRESULT hr = S_FALSE;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	//vertexShader
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };
	//PixelShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };
	//blendState
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ｛形状｝のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//実際に生成
	ID3D12PipelineState* graphicsPipelineState = nullptr;

	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
													 IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
}