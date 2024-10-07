#pragma
#include <Windows.h>
#include "WinApp.h"
#include "System.h"
#include "TestScene.h"

class EngineController{
public:
    EngineController() = default;
    ~EngineController() = default;

    /// <summary>
    /// ‰Šú‰»
    /// </summary>
    /// <param name="hInstance"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    void Initialize(HINSTANCE hInstance, int width, int height);

    /// <summary>
    /// XV
    /// </summary>
    void Run();

    /// <summary>
    /// I—¹ˆ—
    /// </summary>
    void Finalize();

private:
    std::unique_ptr<System> system_;
    std::unique_ptr<TestScene> scene_;
};
