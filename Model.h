#pragma once
#include"ModelData.h"
#include"Material.h"
#include"TransformationMatrix.h"
#include"Transform.h"
#include"Vector4.h"
#include"GameObject.h"

#include<d3d12.h>
#include<wrl.h>
#include<string>


/// <summary>
/// モデル
/// </summary>
class Model{
public:
	Model();
	~Model();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(bool isUseTexture = true);
	/// <summary>
	/// モデルの作成
	/// </summary>
	void Create(const std::string& directoryPath, const std::string& filename,bool isUseTexture = true);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// imguiの更新
	/// </summary>
	void ShowImGuiInterface();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	/// <summary>
	/// リソースの生成
	/// </summary>
	void CreateBuffer();
	/// <summary>
	/// マップする
	/// </summary>
	void Map();
	/// <summary>
	/// モデルデータの取得
	/// </summary>
	/// <returns></returns>
	ModelData GetModelData()const { return modelData; }

	void SetViewProjection(ViewProjection* viewPro);

	void SetPos(const Vector3& pos){ transform.translate = pos; }

private:
	/// <summary>
	/// リソースの生成
	/// </summary>
	void CreateVertexBuffer();
	void CreateMaterialBuffer();
	void CreateMatrixBuffer();
	/// <summary>
	/// マップする
	/// </summary>
	void VertexBufferMap();
	void MaterialBufferMap();
	void MatrixBufferMap();

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

	///=============================================================
	///	texture
	///=============================================================
	D3D12_GPU_DESCRIPTOR_HANDLE handle;

	Vector4 RGBa;
	Transform transform;
	ModelData modelData;
	Material* materialData;
	TransformationMatrix* matrixData;

	int currentLightingMode = 0;

	ViewProjection* viewProjection_;
};

