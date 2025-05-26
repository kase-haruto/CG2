#pragma once

////////////////////////////////////////////////////////////
//	include
////////////////////////////////////////////////////////////
#include <Engine/scene/Base/BaseScene.h>

/* objects */
#include <Engine/Renderer/Sprite/Sprite.h>
#include <Engine/Objects/3D/Actor/BaseGameObject.h>
#include <Game/3dObject/Actor/Player/Player.h>
#include <Game/3dObject/Actor/Enemy/Collection/EnemyCollection.h>

/* graphics */
#include <Engine/Objects/LightObject/DirectionalLight.h>
#include <Engine/Objects/LightObject/PointLight.h>
#include <Game/3d/GameCamera/RailCamera.h>
#include <Engine/Extensions/SkyBox/SkyBox.h>

/* object */
#include <Engine/Renderer/Primitive/PrimitiveDrawer.h>

/* c++ */
#include <vector>
#include <memory>
#include <array>

//デバッグ関連//
#ifdef _DEBUG
#include <Engine/Application/UI/ImGuiManager.h>
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
	/* objects ======================================================*/
	std::unique_ptr<BaseGameObject> modelField_ = nullptr;
	std::unique_ptr<Player> player_ = nullptr;
	std::unique_ptr<EnemyCollection> enemyCollection_ = nullptr;

	std::unique_ptr<SkyBox> skyBox_ = nullptr;
	// カメラ
	std::unique_ptr<RailCamera> railCamera_ = nullptr;

};

