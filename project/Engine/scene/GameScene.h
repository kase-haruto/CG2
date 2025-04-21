#pragma once

////////////////////////////////////////////////////////////
//	include
////////////////////////////////////////////////////////////
#include "Engine/scene/Base/BaseScene.h"

/* objects */
#include <Engine/objects/ModelBuilder.h>
#include <Engine/Editor/UiEditor.h>
#include <Engine/core/UI/ParticleEditPanel.h>
#include <Engine/objects/Sprite.h>  


/* graphics */
#include <Engine/physics/light/DirectionalLight.h>
#include <Engine/physics/light/PointLight.h>
#include <Engine/graphics/FogEffect.h>

/* object */
#include <lib/myFunc/PrimitiveDrawer.h>

/* c++ */
#include <vector>
#include <memory>
#include <array>

//デバッグ関連//
#ifdef _DEBUG
#include<lib/ImGuiManager.h>
#include<Externals/imgui/imgui.h>
#endif // _DEBUG

class GameScene final :
	public BaseScene{
public:
	GameScene();
	GameScene(DxCore* dxCore);
	~GameScene() override = default;

	void Initialize()override;
	void Update()override;
	void CleanUp()override;

private:
	/* graphics =====================================================*/
	std::unique_ptr<FogEffect>fog_ = nullptr;

	/* objects ======================================================*/
	std::unique_ptr<Model> modelField_ = nullptr;

	/* editor =======================================================*/
	std::unique_ptr<ModelBuilder>modelBuilder_ = nullptr;
	std::unique_ptr<UIEditor> uiEditor_ = nullptr;
};

