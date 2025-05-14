#pragma
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/Application/System/System.h>
#include <Engine/Application/Platform/WinApp.h>
#include <Engine/Scene/System/SceneManager.h>
#include <Engine/Application/UI/EngineUI/Renderer/EngineUIRenderer.h>

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

	//ui
	std::unique_ptr<EngineUIRenderer> engineUIRenderer_;

	//scene
	std::unique_ptr<SceneManager> sceneManager_;

};
