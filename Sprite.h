#pragma once
#include<d3d12.h>
#include<wrl.h>
#include"Transform.h"
#include"Matrix4x4.h"
#include"Material.h"

using namespace Microsoft::WRL;

class DirectXCommon;

/// <summary>
/// スプライト
/// </summary>
class Sprite{
private:
	//viewの生成
	D3D12_INDEX_BUFFER_VIEW indexBufferView {};
	ComPtr<ID3D12Resource>vertexResource_;
	ComPtr<ID3D12Resource>indexResource_;
	ComPtr<ID3D12Resource>transformResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite {};

	//directX関連
	ComPtr<ID3D12Device> device_;
	ComPtr<ID3D12GraphicsCommandList>commandList_;
	DirectXCommon* dxCommon_;

	Transform transform_ {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};
	Transform uvTransform {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};
	Matrix4x4* transformData = nullptr;



	//マテリアル用のリソース
	ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_;

public:
	Sprite(DirectXCommon* dx);
	~Sprite();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	/// <summary>
	/// 定数バッファの作成
	/// </summary>
	void CreateBuffer();
	/// <summary>
	/// マップ
	/// </summary>
	void Map();
	void IndexResourceMap();
	void VertexResourceMap();
	void TransformResourceMap();
	void MaterialResourceMap();

	/// <summary>
	/// 定数バッファの取得
	/// </summary>
	/// <returns></returns>
	ComPtr<ID3D12Resource>GetConstBuffer(){ return vertexResource_; }

	void SetRotation(Vector3 rotation){ transform_.rotate = rotation; }
	void SetTranslation(Vector3 translation){ transform_.translate = translation; }	
};

