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

class DirectXCommon final{
private: // メンバ変数
	WinApp* winApp_;
	ID3D12Device* device = nullptr;
	IDXGIAdapter4* useAdapter = nullptr;
	HRESULT hr;
	IDXGIFactory7* dxgiFactory = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	IDXGISwapChain4* swapChain = nullptr;
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	ID3D12Resource* swapChainResources[2] = { nullptr };
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue;
	HANDLE fenceEvent;
	ID3D12Debug1* debugController = nullptr;
	ID3D12InfoQueue* infoQueue = nullptr;

	//三角形の描画について

	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcIncludeHandler* includeHandler = nullptr;
	

	//PSO関連
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	D3D12_BLEND_DESC blendDesc{};
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	IDxcBlob* vertexShaderBlob;
	IDxcBlob* pixelShaderBlob;

	ID3D12Resource* vertexResource = nullptr;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	//ビューポート
	D3D12_VIEWPORT viewport{};
	//シザー矩形
	D3D12_RECT scissorRect{};

	

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static DirectXCommon* GetInstance();

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
	/// レンダーターゲットのクリア
	/// </summary>
	void ClearRenderTarget();

	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns>デバイス</returns>
	ID3D12Device* GetDevice() const { return device; }
	
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
	//ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }

	////---------------------------------------------------////
	///				三角形関連

	/// <summary>
	/// DXCの初期化
	/// </summary>
	void InitializeDXC();


	IDxcBlob* CompileShader(
		//CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		//Compilerに使用するProfile
		const wchar_t* profile,
		//初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcompiler,
		IDxcIncludeHandler* includeHandler
	);

	/// <summary>
	/// vertexResourceの作成
	/// </summary>
	/// <param name="device"></param>
	void CreateVertexResource();

	/// <summary>
	/// vertexBufferViewの作成
	/// </summary>
	void CreateVertexBufferView();

	/// <summary>
	/// Resourceにデータを書き込む
	/// </summary>
	void UploadVertexData();



	void Pipeline();

	void DrawPolygon();

	//=============================================
	//		POS

	void CreateRootSignature();
	void BindInputLayout();
	void CreatePSO();

private: // メンバ関数
	DirectXCommon() = default;
	~DirectXCommon() = default;
	DirectXCommon(const DirectXCommon&) = delete;
	const DirectXCommon& operator=(const DirectXCommon&) = delete;

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

	/// <summary>
	/// レンダーターゲット生成
	/// </summary>
	void CreateFinalRenderTargets();

	/// <summary>
	/// フェンス生成
	/// </summary>
	void CreateFence();

	
};

