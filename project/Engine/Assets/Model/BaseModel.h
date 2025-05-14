#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */

/* engine */
#include <Engine/Assets/Model/ModelData.h>
#include <Engine/core/DirectX/Buffer/DxConstantBuffer.h>
#include <engine/graphics/Material.h>
#include <Engine/Graphics/Pipeline/BlendMode/BlendMode.h>
#include <engine/objects/Transform.h>
#include <Engine/Renderer/Mesh/IMeshRenderable.h>

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
	virtual void UpdateMatrix();
	void UpdateTexture();
	virtual void Map() = 0;
	virtual void ShowImGuiInterface();
	void Draw()override;

	//--------- accessor -----------------------------------------------------
	virtual void SetUvScale(const Vector3& uvScale) = 0;
	virtual void SetColor(const Vector4& color) = 0;
	virtual const Vector4& GetColor() const = 0;
	const WorldTransform& GetWorldTransform(){ return worldTransform_; }

protected:
	//===================================================================*/
	//			protected methods
	//===================================================================*/
	Microsoft::WRL::ComPtr<ID3D12Device>              device_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

	DxConstantBuffer<Material> materialBuffer_;

	std::optional<D3D12_GPU_DESCRIPTOR_HANDLE> handle_ {};

	std::string fileName_;
	std::optional<ModelData> modelData_;
	Material materialData_;
	Material materialParameter_;
	TransformationMatrix matrixData_;

public:
	BlendMode blendMode_ = BlendMode::NORMAL;
	Vector4 RGBa = {1.0f, 1.0f, 1.0f, 1.0f};
	EulerTransform  uvTransform {{1.0f, 1.0f, 1.0f},
							 {0.0f, 0.0f, 0.0f},
							 {0.0f, 0.0f, 0.0f}};

	WorldTransform worldTransform_;

protected:
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureHandles_; // テクスチャハンドルリスト
	float animationSpeed_ = 0.1f; // アニメーションの速度 (秒/フレーム)
	float elapsedTime_ = 0.0f; // 経過時間
	size_t currentFrameIndex_ = 0; // 現在のフレームインデックス

public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
	Matrix4x4 GetWorldRotationMatrix();

protected:
	static const std::string directoryPath_;

	virtual void CreateMaterialBuffer() = 0;
	virtual void MaterialBufferMap() = 0;
};
