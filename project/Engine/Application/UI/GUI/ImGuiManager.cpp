#include "ImGuiManager.h"
/* ========================================================================
/*		include space
/* ===================================================================== */
// engine
#include <Engine/Application/Platform/WinApp.h>
#include <Engine/Foundation/Utility/Func/DxFunc.h>
#include <Engine/Graphics/Descriptor/SrvLocator.h>
#include <Engine/Graphics/Device/DxCore.h>

// externals
#ifdef _DEBUG
#include "imgui/ImGuizmo.h"
#endif // _DEBUG
#include <externals/imgui/imgui_impl_dx12.h>
#include <externals/imgui/imgui_impl_win32.h>


void ImGuiManager::Initialize(WinApp* winApp, const DxCore* dxCore){
	pDxCore_ = dxCore;

	srvHeap_ = CreateDescriptorHeap(pDxCore_->GetDevice().Get(),
									D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
									128,
									true
	);


	//srvの設定
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	// Dockingのみ有効、Viewportsは無効
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; ← この行は削除またはコメントアウト

	ImGui_ImplWin32_Init(winApp->GetHWND());
	ImGui_ImplDX12_Init(pDxCore_->GetDevice().Get(),
						pDxCore_->GetSwapChain().GetSwapChainDesc().BufferCount,
						pDxCore_->GetFormat(),
						srvHeap_.Get(),
						srvHeap_.Get()->GetCPUDescriptorHandleForHeapStart(),
						srvHeap_.Get()->GetGPUDescriptorHandleForHeapStart()
	);
	ImGui::StyleColorsDark(); // ダークテーマを適用

	// fontの設定
	ImFont* font = io.Fonts->AddFontFromFileTTF("Resources/asetts/fonts/FiraMono.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	io.FontDefault = font;
	CustomizeImGuiStyle();

	//先頭にimguiが入ったsrvを管理クラスに移す
	SrvLocator::Provide(srvHeap_, pDxCore_->GetDevice());
}


void ImGuiManager::Finalize(){
	//後始末
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	srvHeap_.Reset();
}

void ImGuiManager::Begin(){
	//フレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#ifdef _DEBUG
	ImGuizmo::BeginFrame();

#endif // _DEBUG


	ComPtr<ID3D12GraphicsCommandList> commandList = pDxCore_->GetCommandList();
	//でスクリプタヒープの配列をセットする
	ID3D12DescriptorHeap* descriptorHeaps[] = {SrvLocator::GetSrvHeap().Get()};
	commandList->SetDescriptorHeaps(1, descriptorHeaps);
}

void ImGuiManager::End(){
	//描画前準備
	ImGui::Render();

}

void ImGuiManager::Draw(){
	ComPtr<ID3D12GraphicsCommandList> commandList = pDxCore_->GetCommandList();
	//描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
}

void ImGuiManager::CustomizeImGuiStyle(){
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = ImVec2(0, 0); // ウィンドウのパディング
	ImVec4* colors = style.Colors;

	// 全体の背景色とウィンドウ背景
	colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.0f); // 非常に暗い背景
	colors[ImGuiCol_ChildBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.0f); // 子ウィンドウ背景
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f); // ポップアップ背景

	// フレーム系
	colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);  // フレーム背景
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);  // フレームホバー時
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);  // フレームアクティブ時

	// ボタン
	colors[ImGuiCol_Button] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);  // 通常ボタン
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);  // ホバー時
	colors[ImGuiCol_ButtonActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);  // クリック時

	// タブ
	colors[ImGuiCol_Tab] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);  // 通常タブ
	colors[ImGuiCol_TabHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);  // ホバー時
	colors[ImGuiCol_TabActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);  // アクティブなタブ
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.05f, 0.05f, 0.05f, 1.0f); // 非フォーカスタブ
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);  // 非フォーカス時のアクティブタブ

	// テキスト
	colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);  // 明るい白グレー
	colors[ImGuiCol_TextDisabled] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);  // 無効化テキスト

	// ボーダーとセパレーター
	colors[ImGuiCol_Border] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);  // 境界線
	colors[ImGuiCol_Separator] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);  // セパレーター
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);  // ホバー時
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);  // アクティブ時

	// スクロールバー
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.0f); // 背景
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);    // 通常時
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f); // ホバー時
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // アクティブ時

	// プログレスバー
	colors[ImGuiCol_PlotLines] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);  // ライン
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);  // ホバー時
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);  // ヒストグラム
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); // ホバー時

	// 選択されていないときの色 (水色を黒系に変更)
	colors[ImGuiCol_Header] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);  // 通常ヘッダー
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);  // ホバー時
	colors[ImGuiCol_HeaderActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);  // アクティブ

	// タイトルバーの色 (アクティブ / 非アクティブ)
	colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.0f); // 非アクティブ時の背景
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);    // アクティブ時の背景
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.02f, 0.02f, 0.02f, 1.0f); // 折りたたみ時の背景

	// ヘッダー (選択された要素やタブの色)
	colors[ImGuiCol_Header] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);  // 通常ヘッダー
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);  // ホバー時
	colors[ImGuiCol_HeaderActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);  // アクティブ状態


	// 丸みを少なくしてフラットな印象に
	style.WindowRounding = 2.0f;  // ウィンドウ角丸
	style.FrameRounding = 2.0f;   // フレーム角丸
	style.GrabRounding = 2.0f;    // スクロールグラブ角丸
}