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

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="hInstance"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    void Initialize(HINSTANCE hInstance);

    /// <summary>
    /// 更新
    /// </summary>
    void Run();

    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize();

private:
    std::unique_ptr<System> system_;
    std::unique_ptr<TestScene> scene_;

    //scene
	std::unique_ptr<SceneManager> sceneManager_;

    std::unique_ptr<EngineUI> ui_;
};
