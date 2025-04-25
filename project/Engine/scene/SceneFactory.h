#pragma once

// engine
#include "Engine/core/DirectX/DxCore.h"

// scene
#include <Engine/scene/Interface/IScene.h>

// lib
#include <memory>

class SceneFactory{
public:
	static std::unique_ptr<IScene> CreateScene(SceneType sceneType, DxCore* dxCore);
};
