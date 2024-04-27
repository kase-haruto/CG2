#pragma once

#ifdef _DEBUG
#include<d3d12.h>
#include<imgui.h>
#include <wrl.h>
#endif // _DEBUG

class WinApp;
class DirectXCommon;

class ImGuiManager{
public:

	static ImGuiManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp"></param>
	/// <param name="dxCommon"></param>
	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// ImGui受付開始
	/// </summary>
	void Begin();

	/// <summary>
	/// ImGui受付終了
	/// </summary>
	void End();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
#ifdef _DEBUG
	// DirectX基盤インスタンス（借りてくる）
	DirectXCommon* dxCommon_ = nullptr;
	// SRV用ヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
#endif // _DEBUG

private:
	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = default;
	const ImGuiManager& operator=(const ImGuiManager&) = delete;	
};

