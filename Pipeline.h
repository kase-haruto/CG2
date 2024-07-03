#pragma once
#include<d3d12.h>
#include<dxcapi.h>
#include"Matrix4x4.h"

class Pipeline{
private:
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;

	D3D12_BLEND_DESC blendDesc {};

	IDxcBlob* vertexShaderBlob;
	IDxcBlob* pixelShaderBlob;

	ID3D12Resource* vertexResource = nullptr;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView {};
	//ビューポート
	D3D12_VIEWPORT viewport {};
	//シザー矩形
	D3D12_RECT scissorRect {};

	ID3D12Resource* materialResource;
	Matrix4x4* wvpData = nullptr;
	ID3D12Resource* wvpResource;

public:
	Pipeline();
	~Pipeline();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 設定
	/// </summary>
	void Apply();
	/// <summary>
	/// Dxcの初期化
	/// </summary>
	void InitializeDXC();
	/// <summary>
	/// rootSignatureの設定
	/// </summary>
	void CreateRootSignature();
	/// <summary>
	/// PSOを作成
	/// </summary>
	void CreatePSO();

};

