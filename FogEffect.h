#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<wrl.h>

using namespace DirectX;
using namespace Microsoft::WRL;

//霧のパラメータを表す構造体
struct FogParameters{
	float fogStart;
	float fogEnd;
	XMFLOAT4 fogColor;
};

class FogEffect{
private:
	//霧のパラメータ
	FogParameters parameters;
	//霧のパラメータを保有する定数バッファ
	ComPtr<ID3D12Resource> constantBuffer;
	UINT8* mappedConstantBuffer;
	UINT bufferSize;

public:
	//コンストラクタ
	FogEffect(int32_t width, int32_t height, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	//デストラクタ
	~FogEffect();

	//霧のパラメータを呼応審
	void Update();

	//霧の効果を適用
	void Apply(ID3D12GraphicsCommandList* commandList, ID3D12Device* device);
};

