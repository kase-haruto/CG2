#pragma once
#include <Windows.h>
#include <chrono>
#include <cstdlib>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "WinApp.h"
#include<dxcapi.h>
#include"TriangleRender.h"
#include"Matrix4x4.h"
#include"ShaderManager.h"

class ImGuiManager;

class DirectXCommon final{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private: // メンバ変数

	uint32_t bufferWidth_;
	uint32_t bufferHeight_;

	WinApp* winApp_;
	ComPtr<ID3D12Device> device = nullptr;
	IDXGIAdapter4* useAdapter = nullptr;
	HRESULT hr;
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	IDXGISwapChain4* swapChain = nullptr;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc {};
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr;
	ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };
	ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue;
	HANDLE fenceEvent;
	ComPtr<ID3D12Debug1> debugController = nullptr;
	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc {};

	ComPtr<ID3D12Resource> depthStencilResource;
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

	D3D12_BLEND_DESC blendDesc{};

	
	//ビューポート
	D3D12_VIEWPORT viewport{};
	//シザー矩形
	D3D12_RECT scissorRect{};


	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;

	bool useMonsterBall = true;



public:
	DirectXCommon(){}
	~DirectXCommon(){}

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(
		WinApp* win,uint32_t width, uint32_t height);

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();
	
	/// <summary>
	/// viewPortとscissorのセット
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void SetViewPortAndScissor(uint32_t width, uint32_t height);

	/// <summary>
	/// 描画コマンドリストの取得
	/// </summary>
	/// <returns>描画コマンドリスト</returns>
	const ComPtr<ID3D12GraphicsCommandList>& GetCommandList() const { return commandList; }

	
	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// ディスクリプタヒープの生成
	/// </summary>
	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ID3D12Device* device,
											   D3D12_DESCRIPTOR_HEAP_TYPE heapType,
											   UINT numDescriptors,
											   bool shaderVisible);
	
	//================
	//アクセッサ
	//================
	
	/// <summary>
	/// デバイスの取得
	/// </summary>
	const ComPtr<ID3D12Device>& GetDevice()const{ return device; }

	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc()const{ return swapChainDesc; }
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc()const{ return rtvDesc; }
	void SetCommandList(ID3D12GraphicsCommandList* comandList){ commandList = comandList; }

	uint32_t GetBufferWidth()const{ return bufferWidth_; }
	uint32_t GetBufferHeight()const{ return bufferHeight_; }


	const ComPtr<ID3D12DescriptorHeap>& GetRtvDescriptorHeap()const{ return rtvDescriptorHeap; }
	const ComPtr<ID3D12DescriptorHeap>& GetSrvDescriptorHeap()const{ return dsvDescriptorHeap; }

	uint32_t GetDescriptorSizeSRV()const{ return descriptorSizeSRV; }
	uint32_t GetDescriptorSizeRTV()const{ return descriptorSizeRTV; }
	uint32_t GetDescriptorSizeDSV()const{ return descriptorSizeDSV; }

private: // メンバ関数



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
	/// レンダーターゲット生成
	/// </summary>
	void CreateFinalRenderTargets();

	/// <summary>
	/// フェンス生成
	/// </summary>
	void CreateFence();
};

