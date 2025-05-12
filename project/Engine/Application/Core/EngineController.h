#pragma
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/core/WinApp.h>
#include <Engine/core/System.h>
#include <Engine/scene/SceneManager.h>

// c++
#include <Windows.h>

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
