﻿#pragma once
#include<d3d12.h>
#include<wrl.h>
#include"Transform.h"
#include"Matrix4x4.h"
using namespace Microsoft::WRL;

class DirectXCommon;

/// <summary>
/// スプライト
/// </summary>
class Sprite{
private:
	ComPtr<ID3D12Resource>vertexResource_;
	ComPtr<ID3D12Resource>transformResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite {};

	//directX関連
	ComPtr<ID3D12Device> device_;
	ComPtr<ID3D12GraphicsCommandList>commandList_;
	DirectXCommon* dxCommon_;

	Transform transform_ {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};
	Matrix4x4* transformData = nullptr;

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
	void VertexResourceMap();
	void TransformResourceMap();

	/// <summary>
	/// 定数バッファの取得
	/// </summary>
	/// <returns></returns>
	ComPtr<ID3D12Resource>GetConstBuffer(){ return vertexResource_; }

	void SetRotation(Vector3 rotation){ transform_.rotate = rotation; }
	void SetTranslation(Vector3 translation){ transform_.translate = translation; }	
};
