#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
/* engine */
#include <Engine/Extensions/Fog/FogEffect.h>
#include <Engine/scene/Base/BaseScene.h>

/* c++ */
#include <memory>
#include <vector>

///デバッグ関連///
#ifdef _DEBUG

#include <externals/imgui/imgui.h>
#endif // _DEBUG

/* ========================================================================
/* testScene
/* ===================================================================== */
class TestScene final :
	public BaseScene{
public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
	TestScene();
	~TestScene() override = default;

	void Initialize()override;
	void Update()override;
	void CleanUp()override;
	void LoadAssets()override;
private:
	/* graphics =====================================================*/
	std::unique_ptr<FogEffect>fog_ = nullptr;

	/* objects ====================================================*/


};

