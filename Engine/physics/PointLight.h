#pragma once

#include"Vector4.h"
#include"Vector3.h"

#include<wrl.h>
#include<d3d12.h>

struct PointLightData{
	Vector4 color;		//ライトの色
	Vector3 position;	//ライトの位置
	float intensity;	//光度
	float radius;		//ライトの届く最大距離
	float decay;		//減衰率
	float pad[2];
};

class DirectXCommon;

class PointLight{
public:
	PointLight();
	~PointLight();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Render();
	/// <summary>
	/// バッファの生成
	/// </summary>
	void CreateBuffer();
	/// <summary>
	/// マップ
	/// </summary>
	void Map();

	void SetRootSignature(const Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

private:
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>commandList_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device>device_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	PointLightData* data_;

	Vector4 color_ = {1.0f,1.0f,1.0f,1.0f};
};

