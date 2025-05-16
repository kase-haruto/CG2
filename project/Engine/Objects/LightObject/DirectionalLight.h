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
#include <Engine/Graphics/Buffer/DxConstantBuffer.h>

/* config */
#include <Data/Engine/Configs/Scene/Objects/LightObjects/DirectionalLightConfig.h>

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
	DirectionalLight(const std::string& name);
	DirectionalLight() = default;
	~DirectionalLight();

	void Initialize();
	void Update()override;
	void SetCommand(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, PipelineType type);
	void ShowGui()override;

	void SaveConfig(const std::string& path)override;
	void LoadConfig(const std::string& path)override;

private:
	DxConstantBuffer<DirectionalLightData> constantBuffer_;
	DirectionalLightConfig config_;
};

