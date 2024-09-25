#pragma once
#include <d3dcompiler.h>
#include<d3d12.h>
#include"ShaderCompiler.h"

class PipelineStateObject final{
private://メンバ変数
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	D3D12_BLEND_DESC blendDesc{};
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	IDxcBlob* vertexShaderBlob;
	IDxcBlob* pixelShaderBlob;
	
public://メンバ関数

	PipelineStateObject() {}
	~PipelineStateObject() {
		signatureBlob->Release();
		if (errorBlob) {
			errorBlob->Release();
		}
		rootSignature->Release();
		pixelShaderBlob->Release();
		vertexShaderBlob->Release();
	}

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

