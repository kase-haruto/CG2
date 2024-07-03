#pragma once
#include"DirectXCommon.h"
#include"WinApp.h"

#ifdef _DEBUG
#include"ImGuiManager.h"
#endif // _DEBUG


#include<stdint.h>

class System{
public:
	System();
	~System();

	void Initialize(int32_t clientWidth,int32_t clientHeight);
	void Finalize();

	int ProcessMessage();

private:
	WinApp* winApp_;
	DirectXCommon* dxCommon_;
#ifdef _DEBUG
	// ImGuiの初期化
	ImGuiManager* imguiManager_;
#endif // _DEBUG
};

