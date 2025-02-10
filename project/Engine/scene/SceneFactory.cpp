#include "SceneFactory.h"

//scene
#include "GameScene.h"
#include "TestScene.h"

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