#pragma once
#include <Windows.h>
#include <chrono>
#include <cstdlib>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "WinApp.h"

class DirectX12 {
private: // メンバ変数
	WinApp* winApp_;
	ID3D12Device* device = nullptr;
	HRESULT hr;
	IDXGIFactory7* dxgiFactory = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	IDXGISwapChain4* swapChain = nullptr;
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static DirectX12* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(
		WinApp* win, int32_t backBufferWidth = WinApp::kWindowWidth,
		int32_t backBufferHeight = WinApp::kWindowHeight);

	/// <summary>
	/// 描画コマンドリストの取得
	/// </summary>
	/// <returns>描画コマンドリスト</returns>
	//ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }


private: // メンバ関数
	DirectX12() = default;
	~DirectX12() = default;
	DirectX12(const DirectX12&) = delete;
	const DirectX12& operator=(const DirectX12&) = delete;

	/// <summary>
	/// DXGIデバイス初期化
	/// </summary>
	void InitializeDXGIDevice();


	/// <summary>
	/// コマンド関連初期化
	/// </summary>
	void InitializeCommand();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// ディスクリプタヒープの生成
	/// </summary>
	void CreateDescriptorHeap();
};

