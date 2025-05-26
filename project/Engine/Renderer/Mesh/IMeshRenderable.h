#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
#include <Engine/Foundation/Math/Matrix4x4.h>
#include <Engine/objects/Transform/Transform.h>
#include <Engine/Graphics/Pipeline/BlendMode/BlendMode.h>
#include <Engine/System/Observer/SceneObject/IRenderableDeathListener.h>

#include <d3d12.h>
#include <wrl.h>
#include <vector>

/* ========================================================================
/* メッシュ描画用インターフェース
/* ===================================================================== */
class IMeshRenderable {
public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
	virtual ~IMeshRenderable();


		// 監視者の登録
	void AddDeathListener(IRenderableDeathListener* listener) {
		deathListeners_.push_back(listener);
	}

	// 監視者の解除
	void RemoveDeathListener(IRenderableDeathListener* listener);

	virtual BlendMode GetBlendMode() const = 0;
	virtual void Draw(const WorldTransform& transform) = 0;

private:
	//===================================================================*/
	//			private methods
	//===================================================================*/
	std::vector<IRenderableDeathListener*> deathListeners_;
};

