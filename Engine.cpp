#include "Engine.h"

Engine::Engine(){}

Engine::~Engine(){}

Engine* Engine::GetInstance(){
	static Engine instance;
	return& instance;
}

void Engine::Initialize(int32_t clientWidth, int32_t clientHeight){
	winApp_->GetInstance();
	dxCommon_->GetInstance();
	dxCommon_->Initialize(winApp_, 1280, 720);
	dxCommon_->Pipeline();

#ifdef _DEBUG
	imguiManager_->Initialize(winApp_, dxCommon_);
#endif // _DEBUG

}

void Engine::Finalize(){
	imguiManager_->Finalize();
	imguiManager_ = nullptr;
	dxCommon_->Finalize();
	dxCommon_ = nullptr;

	//ウィンドウの破棄
	winApp_->TerminateGameWindow();
}

int Engine::ProcessMessage(){ return winApp_->ProcessMessage() ? 1 : 0; }

