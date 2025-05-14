#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */

/* math */
#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Foundation/Math/Vector4.h>

/* engine */
#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <Engine/Graphics/Pipeline/PipelineType.h>
#include <Engine/Lighting/LightData.h>

/* c++ */
#include<d3d12.h>
#include<wrl.h>

struct DirectionalLightData{
	Vector4 color;		//ライトの色
	Vector3 direction;	//ライトの向き
	float intensity;	//輝度
};

class DxCore;

class DirectionalLight
	:public SceneObject{
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
	/// バッファの生成
	/// </summary>
	void CreateBuffer();
	/// <summary>
	/// マップ
	/// </summary>
	void Map();

	void SetCommand(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, PipelineType type);

	void ShowGui()override;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	DirectionalLightData* data_;

	Vector4 color_ = {1.0f,1.0f,1.0f,1.0f};

	const DxCore* pDxCore_ = nullptr;
};

