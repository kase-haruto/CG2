#include "SceneFactory.h"

//scene
#include <Engine/Scene/Game/GameScene.h>
#include <Engine/Scene/Test/TestScene.h>

std::unique_ptr<IScene> SceneFactory::CreateScene(SceneType sceneType, DxCore* dxCore){
    switch (sceneType){
        case SceneType::PLAY:
            return std::make_unique<GameScene>(dxCore);
        case SceneType::TEST:
            return std::make_unique<TestScene>(dxCore);
        default:
            return nullptr;
    }
}