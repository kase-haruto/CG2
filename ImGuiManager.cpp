﻿#include "ImGuiManager.h"

#ifdef _DEBUG
#include "DirectXCommon.h"
#include "WinApp.h"
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#endif

ImGuiManager* ImGuiManager::GetInstance(){
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize(WinApp* winApp, DirectXCommon* dxCommon){
#ifdef _DEBUG
	dxCommon_ = dxCommon;
	srvHeap_ = dxCommon_->CreateDescriptorHeap(dxCommon_->GetDevice(),
											   D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
											   128,
											   true
	);
	commandList_ = dxCommon_->GetCommandList();

	//srvの設定
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHWND());
	ImGui_ImplDX12_Init(dxCommon_->GetDevice(),
						dxCommon_->GetSwapChainDesc().BufferCount,
						dxCommon->GetRtvDesc().Format,
						srvHeap_.Get(),
						srvHeap_.Get()->GetCPUDescriptorHandleForHeapStart(),
						srvHeap_.Get()->GetGPUDescriptorHandleForHeapStart()
	);
#endif // _DEBUG
}

void ImGuiManager::Finalize(){
#ifdef _DEBUG
	//後始末
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif // _DEBUG
}

void ImGuiManager::Begin(){
#ifdef _DEBUG
	//フレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif // _DEBUG
}

void ImGuiManager::End(){
#ifdef _DEBUG
	//ImHui内部コマンドを生成
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList_.Get());
#endif // _DEBUG
}

void ImGuiManager::Draw(){
#ifdef _DEBUG
	ImGui::Render();
	//でスクリプタヒープの配列をセットする
	ID3D12DescriptorHeap* descriptorHeaps[] = {srvHeap_.Get()};
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);	
#endif // _DEBUG
}