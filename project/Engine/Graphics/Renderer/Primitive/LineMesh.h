#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/Graphics/Renderer/Primitive/PrimitiveMesh.h>
#include <Engine/core/DirectX/Buffer/DxIndexBuffer.h>
#include <Engine/core/DirectX/Buffer/DxVertexBuffer.h>
#include <Engine/core/DirectX/Buffer/DxConstantBuffer.h>
#include <Engine/graphics/VertexData.h>
#include <Engine/graphics/Material.h>
#include <Engine/objects/Transform.h>

struct Vector3;
struct Matrix4x4;

class LineMesh 
	: public IPrimitiveMesh{
public:
	LineMesh(const Vector3& start, const Vector3& end);
	~LineMesh() = default;

	void Draw() override;

	void SetColor(const Vector4& color) override;
	const Matrix4x4& GetWorldMatrix() const override;
	void SetBlendMode(BlendMode mode) override;
	BlendMode GetBlendMode() const override;

private:
	static void InitializeSharedBuffer();

private:
	static inline DxVertexBuffer<VertexPosColor> sharedVertexBuffer_;
	static inline bool sharedInitialized_ = false;

	WorldTransform transform_; // ← あなたのWorldTransformクラス
	Vector4 color_ = {1, 1, 1, 1};
	BlendMode blendMode_ = BlendMode::NORMAL;

	Vector3 start_;
	Vector3 end_;
};