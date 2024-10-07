#include "core/EngineController.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int){
	EngineController EngineController;

	EngineController.Initialize(hInstance);
	EngineController.Run();
	EngineController.Finalize();

	return 0;
}