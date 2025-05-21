#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */

/* engine */
#include <Engine/Assets/Model/ModelData.h>
#include <Engine/Graphics/Buffer/DxConstantBuffer.h>
#include <engine/graphics/Material.h>
#include <Engine/Graphics/Pipeline/BlendMode/BlendMode.h>
#include <Engine/Objects/Transform/Transform.h>
#include <Engine/Renderer/Mesh/IMeshRenderable.h>

/*data*/
#include <Data/Engine/Configs/Scene/Objects/Model/BaseModelConfig.h>

/* math */
#include <Engine/Foundation/Math/Vector4.h>

/* c++ */
#include <d3d12.h>
#include <memory>
#include <string>
#include <wrl.h>

/* ========================================================================
/*		model
/* ===================================================================== */
class BaseModel
	:public IMeshRenderable{
public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
	virtual ~BaseModel() = default;

	virtual void Initialize() = 0;
	virtual void Update();
	virtual void OnModelLoaded();
	void UpdateTexture();
	virtual void Map() = 0;
	virtual void ShowImGuiInterface();
	void Draw(const WorldTransform& transform)override;

	//--------- config -----------------------------------------------------
	void ApplyConfig();
	void ExtractConfig();

	//--------- accessor -----------------------------------------------------
	const Vector4& GetColor() const { return materialData_.color; }
	void SetColor(const Vector4& color) { materialData_.color = color; }

protected:
	//===================================================================*/
	//			protected methods
	//===================================================================*/
	DxConstantBuffer<Material> materialBuffer_;

	std::optional<D3D12_GPU_DESCRIPTOR_HANDLE> handle_ {};

	std::string fileName_;
	std::optional<ModelData> modelData_;
	Material materialData_;
public:
	BlendMode blendMode_ = BlendMode::NORMAL;
	Transform2D  uvTransform {{1.0f, 1.0f},
							 0.0f,
							 {0.0f, 0.0f}};

	BaseModelConfig config_;


protected:
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureHandles_; // テクスチャハンドルリスト
	float animationSpeed_ = 0.1f; // アニメーションの速度 (秒/フレーム)
	float elapsedTime_ = 0.0f; // 経過時間
	size_t currentFrameIndex_ = 0; // 現在のフレームインデックス

protected:
	static const std::string directoryPath_;

	virtual void CreateMaterialBuffer() = 0;
	virtual void MaterialBufferMap() = 0;
};
