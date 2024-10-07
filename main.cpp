#include<Windows.h>
#include "core/EngineController.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int){
	EngineController EngineController;

	EngineController.Initialize(hInstance, 1280, 720);
	EngineController.Run();
	EngineController.Finalize();

	return 0;
}