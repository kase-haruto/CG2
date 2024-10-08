﻿#include "ImGuiManager.h"

#include "core/DirectX/DxCore.h"
#include "WinApp.h"
#include "myFunc/DxFunc.h"

#ifdef _DEBUG

#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#endif

#include"SrvLocator.h"

void ImGuiManager::Initialize(WinApp* winApp, const DxCore* dxCore){
#ifdef _DEBUG
	pDxCore_ = dxCore;

	srvHeap_ = CreateDescriptorHeap(pDxCore_->GetDevice().Get(),
									D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
									128,
									true
	);
	

	//srvの設定
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// Docking and Multi-Viewport feature enable
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplWin32_Init(winApp->GetHWND());
	ImGui_ImplDX12_Init(pDxCore_->GetDevice().Get(),
						pDxCore_->GetSwapChain().GetSwapChainDesc().BufferCount,
						pDxCore_->GetRenderTarget().rtvDesc_.Format,
						srvHeap_.Get(),
						srvHeap_.Get()->GetCPUDescriptorHandleForHeapStart(),
						srvHeap_.Get()->GetGPUDescriptorHandleForHeapStart()
	);

	// Customize ImGui style when viewports are enabled
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable){
		ImGuiStyle& style = ImGui::GetStyle();
		if (style.WindowRounding == 0.0f){
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	}

	//先頭にimguiが入ったsrvを管理クラスに移す
	SrvLocator::Provide(srvHeap_, pDxCore_->GetDevice());
#endif // _DEBUG
}

void ImGuiManager::Finalize(){
#ifdef _DEBUG
	//後始末
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	srvHeap_.Reset();
#endif // _DEBUG
}

void ImGuiManager::Begin(){
#ifdef _DEBUG
	//フレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ComPtr<ID3D12GraphicsCommandList> commandList = pDxCore_->GetCommandList();
	//でスクリプタヒープの配列をセットする
	ID3D12DescriptorHeap* descriptorHeaps[] = {SrvLocator::GetSrvHeap().Get()};
	commandList->SetDescriptorHeaps(1, descriptorHeaps);
#endif // _DEBUG
}

void ImGuiManager::End(){
#ifdef _DEBUG
	//描画前準備
	ImGui::Render();

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable){
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
#endif // _DEBUG
}

void ImGuiManager::Draw(){
#ifdef _DEBUG
	ComPtr<ID3D12GraphicsCommandList> commandList = pDxCore_->GetCommandList();
	//描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

#endif // _DEBUG
}
