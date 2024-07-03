#pragma once
#include"DirectXCommon.h"
#include"WinApp.h"

#ifdef _DEBUG
#include"ImGuiManager.h"
#endif // _DEBUG


#include<stdint.h>

class Engine final{
public:
	static Engine* GetInstance();

	

	void Initialize(int32_t clientWidth,int32_t clientHeight);
	void Finalize();

	int ProcessMessage();

private:
	Engine();
	~Engine();
	//コピーコンストラクタの禁止
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;


private:
	WinApp* winApp_;
	DirectXCommon* dxCommon_;
#ifdef _DEBUG
	// ImGuiの初期化
	ImGuiManager* imguiManager_;
#endif // _DEBUG
};

