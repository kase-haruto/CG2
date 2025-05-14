#pragma
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/Application/System/System.h>
#include <Engine/Application/Platform/WinApp.h>
#include <Engine/Scene/System/SceneManager.h>

// c++
#include <Windows.h>

class EngineController {
public:
	EngineController() = default;
	~EngineController() = default;

	void Initialize(HINSTANCE hInstance);
	bool Update();
	void Render();
	void Run();
	void Finalize();

private:
	std::unique_ptr<System> system_;

	//scene
	std::unique_ptr<SceneManager> sceneManager_;

};
