#pragma
#include <Windows.h>
#include "WinApp.h"
#include "System.h"
#include "../scene/SceneManager.h"
#include "../scene/GameScene.h"
#include "../scene/TestScene.h"
#include "Engine/core/EngineUI.h"

class EngineController{
public:
    EngineController() = default;
    ~EngineController() = default;

    void Initialize(HINSTANCE hInstance);
    void Run();
    void Finalize();

private:
    std::unique_ptr<System> system_;

    //scene
	std::unique_ptr<SceneManager> sceneManager_;

};
