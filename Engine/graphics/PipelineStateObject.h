#pragma once
#include <d3dcompiler.h>
#include<d3d12.h>
#include <wrl.h>
#include"ShaderCompiler.h"

using Microsoft::WRL::ComPtr;

class PipelineStateObject final{
private://メンバ変数
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc {};
	D3D12_BLEND_DESC blendDesc {};
	D3D12_RASTERIZER_DESC rasterizerDesc {};

	ComPtr<IDxcBlob> vertexShaderBlob;
	ComPtr<IDxcBlob> pixelShaderBlob;
	
public://メンバ関数

	PipelineStateObject() {}
	~PipelineStateObject() {}

	/// <summary>
	/// RootSignatureの生成
	/// </summary>
	void CreateRootSignature(ID3D12Device* device);
	
	/// <summary>
	/// InputLayoutの設定を行う
	/// </summary>
	void BindInputLayout();

	/// <summary>
	/// BlendStateの設定を行う
	/// </summary>
	void BindBlendState();

	/// <summary>
	/// RasterizerStateの設定を行う
	/// </summary>
	void BintRasterizerState();

	/// <summary>
	/// shaderをコンパイルする
	/// </summary>
	void CompileShader(ShaderCompiler* sc);

	/// <summary>
	/// csoを生成する
	/// </summary>
	void CreatePSO(ID3D12Device* device);

	void Update();

};

