#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/objects/LightObject/DirectionalLight.h>
#include <Engine/objects/LightObject/PointLight.h>

#include <memory>

class LightLibrary{
public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
	LightLibrary(class SceneObjectLibrary* library);
	LightLibrary() = default;
	~LightLibrary() = default;

	void Update();
	void Clear();
	void SetDirectionalLight(DirectionalLight* light);
	void SetPointLight(PointLight* light);

	DirectionalLight* GetDirectionalLight() const{ return directionalLight_; }
	PointLight* GetPointLight() const{ return pointLight_; }

	void SetCommand(ID3D12GraphicsCommandList* cmdList, PipelineType pipelineType);
	void SetCommand(ID3D12GraphicsCommandList* cmdList,
					PipelineType pipelineType,
					LightType lightType);
private:
	//===================================================================*/
	//			private methods
	//===================================================================*/
	DirectionalLight* directionalLight_ = nullptr;  // 所有しない生ポインタ
	PointLight* pointLight_ = nullptr;              // 所有しない生ポインタ

};

