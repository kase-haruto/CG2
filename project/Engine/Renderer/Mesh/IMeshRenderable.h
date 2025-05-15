#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/Foundation/Math/Matrix4x4.h>
#include <Engine/objects/Transform/Transform.h>

#include <d3d12.h>
#include <wrl.h>

/* ========================================================================
/* メッシュ描画用インターフェース
/* ===================================================================== */
class IMeshRenderable{
public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
	virtual ~IMeshRenderable() = default;

	virtual void Draw(const WorldTransform& transform) = 0;
};

