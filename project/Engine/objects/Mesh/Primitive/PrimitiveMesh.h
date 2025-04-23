#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
/* engine */
#include <engine/objects/Mesh/IMeshRenderable.h>
#include <Engine/graphics/blendMode/BlendMode.h>

#include <lib/myMath/Vector4.h>
#include <lib/myMath/Matrix4x4.h>
/* ========================================================================
/* primitiveObject
/* ===================================================================== */
class IPrimitiveMesh :
	public IMeshRenderable{
public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
	IPrimitiveMesh() = default;

	//--------- accessor -----------------------------------------------------
	//material
	virtual void SetColor(const Vector4& color) = 0;

	virtual void SetBlendMode(BlendMode mode) = 0;
	virtual BlendMode GetBlendMode() const = 0;
	virtual const Matrix4x4& GetWorldMatrix() const =0;
private:
	//===================================================================*/
	//			private functions
	//===================================================================*/
};

