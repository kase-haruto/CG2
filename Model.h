#pragma once
#include"ModelData.h"
#include"Material.h"
#include"TransformationMatrix.h"
#include"Transform.h"
#include"ViewProjection.h"
#include"Vector4.h"

#include<d3d12.h>
#include<wrl.h>

class DirectXCommon;

/// <summary>
/// モデル
/// </summary>
class Model{
private:
	///=============================================================
	///	directX関連
	///=============================================================
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>commandList_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>pipelineState_;

	///=============================================================
	///	Resources
	///=============================================================
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView {};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;

	Vector4 RGBa;
	Transform transform;
	ModelData modelData;
	Material* materialData;
	TransformationMatrix* matrixData;

	ViewProjection* viewProjection;
public:
	Model();
	~Model();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	/// <summary>
	/// リソースの生成
	/// </summary>
	void CreateBuffer();
	void CreateVertexBuffer();
	void CreateMaterialBuffer();
	void CreateMatrixBuffer();
	/// <summary>
	/// マップする
	/// </summary>
	void Map();
	void VertexBufferMap();
	void MaterialBufferMap();
	void MatrixBufferMap();

};

