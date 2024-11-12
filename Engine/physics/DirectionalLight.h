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

// ライティングモードの定義
enum LightingMode{
	HalfLambert = 0,
	Lambert,
	specularReflection,
	NoLighting,
};

class DxCore;

class DirectionalLight{
public:
	DirectionalLight();
	~DirectionalLight();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCore"></param>
	void Initialize(const DxCore* dxCore);
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

	void ShowImGuiInterFace();

	void SetRootSignature(const Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	DirectionalLightData* data_;


	Vector4 color_ = {1.0f,1.0f,1.0f,1.0f};

	const DxCore* pDxCore_ = nullptr;

public:
	DirectionalLightData parameter_;

};

