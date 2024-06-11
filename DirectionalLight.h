#pragma once
#include"Vector4.h"
#include"Vector3.h"

#include<wrl.h>
#include<d3d12.h>

struct DirectionalLightData{
	Vector4 color;		//ライトの色
	Vector3 direction;	//ライトの向き
	float intensity;	//輝度
};

class DirectXCommon;

class DirectionalLight{
private:
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>commandList_;
	Microsoft::WRL::ComPtr<ID3D12Device>device_;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	DirectionalLightData* data_;
public:
	DirectionalLight();
	~DirectionalLight();

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

};

