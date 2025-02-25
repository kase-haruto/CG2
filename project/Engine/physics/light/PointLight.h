#pragma once

/* math */
#include "lib/myMath/Vector3.h"
#include "lib/myMath/Vector4.h"

/* engine */
#include "Engine/graphics/Pipeline/PipelineType.h"
#include "Engine/objects/SceneObject.h"

/* lib */
#include <wrl.h>
#include <d3d12.h>

struct PointLightData{
	Vector4 color;		//ライトの色
	Vector3 position;	//ライトの位置
	float intensity;	//光度
	float radius;		//ライトの届く最大距離
	float decay;		//減衰率
	float pad[2];
};

class DxCore;

class PointLight
: public SceneObject{
public:
	PointLight();
	~PointLight();

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
	/// バッファの生成
	/// </summary>
	void CreateBuffer();
	/// <summary>
	/// マップ
	/// </summary>
	void Map();

	void ShowGui()override;

	void SetRootSignature(const Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

	void SetCommand(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, PipelineType type);

	void SetPosition(const Vector3& position);

private:
	const DxCore* pDxCore_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	PointLightData* data_;

	Vector4 color_ = {1.0f,1.0f,1.0f,1.0f};
};

