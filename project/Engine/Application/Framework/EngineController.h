#pragma
/* ========================================================================
/* include space
/* ===================================================================== */
// engine
#include <Engine/Application/System/System.h>
#include <Engine/Application/Platform/WinApp.h>
#include <Engine/Scene/System/SceneManager.h>
#include <Engine/Application/UI/EngineUI/Core/EngineUICore.h>
#include <Engine/Graphics/Core/GraphicsSystem.h>
#include <Engine/Editor/Collection/EditorCollection.h>

// c++
#include <Windows.h>

class EngineController {
public:
	EngineController() = default;
	~EngineController() = default;

	void Initialize(HINSTANCE hInstance);
	bool Update();
	void BeginUpdate();
	void EndUpdate();
	void Render();
	void Run();
	void Finalize();

private:
	std::unique_ptr<System> system_;
	std::unique_ptr<GraphicsSystem> graphicsSystem_;

	//ui
	std::unique_ptr<EngineUICore> engineUICore_;

	//scene
	std::unique_ptr<SceneManager> sceneManager_;
	std::unique_ptr<EditorCollection> editorCollection_;

};
