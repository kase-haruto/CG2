#define ENGINE_EXPORTS 
#include "EngineMain.h"
#include "../EngineController.h"
#include <memory>

static std::unique_ptr<EngineController> engine;

void Engine_Initialize(HINSTANCE hInstance){
	engine = std::make_unique<EngineController>();
	engine->Initialize(hInstance);
}

bool Engine_Update(){
	return engine->Update(); //falseで終了
}

void Engine_Render(){
	engine->Render();
}

void Engine_Finalize(){
	if (engine){
		engine->Finalize();
		engine.reset();
	}
}
