#pragma once
#include <Windows.h>
#include "EngineAPI.h"

extern "C" {
    ENGINE_API void Engine_Initialize(HINSTANCE hInstance);
    ENGINE_API bool Engine_Update();
    ENGINE_API void Engine_Render();
    ENGINE_API void Engine_Finalize();
}