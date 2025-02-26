#pragma once

////////////////////////////////////////////////////////////
//	include
////////////////////////////////////////////////////////////
#include "IScene.h"

/* objects */
#include "engine/objects/ModelBuilder.h"
#include "engine/Editor/UiEditor.h"
#include "Engine/objects/Sprite.h"  

/* graphics */
#include "engine/physics/light/DirectionalLight.h"
#include "engine/physics/light/PointLight.h"
#include "engine/graphics/FogEffect.h"

/* object */
#include "lib/myFunc/PrimitiveDrawer.h"

/* c++ */
#include <vector>
#include <memory>
#include <array>

//デバッグ関連//
#ifdef _DEBUG
#include"lib/ImGuiManager.h"
#include"externals/imgui/imgui.h"
#endif // _DEBUG

class GameScene final :
	public IScene{
public:
	GameScene();
	GameScene(DxCore* dxCore);
	~GameScene() override = default;

	void Initialize()override;
	void Update()override;
	void Draw()override;
	void CleanUp()override;

private:
	/* graphics =====================================================*/
	std::unique_ptr<FogEffect>fog_ = nullptr;

	/* objects ====================================================*/
	std::unique_ptr<Model> modelField_ = nullptr;

	/* editor =====================================================*/
	std::unique_ptr<ModelBuilder>modelBuilder_ = nullptr;
	std::unique_ptr<UIEditor> uiEditor_ = nullptr;
};

