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
#include"Transform.h"
#include"Matrix4x4.h"
#include"ViewProjection.h"
#include"VertexData.h"

class ImGuiManager;
class FogEffect;

class DirectXCommon final{

private: // メンバ変数

	uint32_t bufferWidth_;
	uint32_t bufferHeight_;

	WinApp* winApp_;
	ID3D12Device* device = nullptr;
	IDXGIAdapter4* useAdapter = nullptr;
	HRESULT hr;
	IDXGIFactory7* dxgiFactory = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	IDXGISwapChain4* swapChain = nullptr;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc {};
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	ID3D12Resource* swapChainResources[2] = { nullptr };
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue;
	HANDLE fenceEvent;
	ID3D12Debug1* debugController = nullptr;
	ID3D12InfoQueue* infoQueue = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc {};

	ID3D12Resource* depthStencilResource;
	ID3D12DescriptorHeap* dsvDescriptorHeap;

	//三角形の描画について
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcIncludeHandler* includeHandler = nullptr;
	

	//PSO関連
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc {};
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;

	D3D12_BLEND_DESC blendDesc{};

	IDxcBlob* vertexShaderBlob;
	IDxcBlob* pixelShaderBlob;

	ID3D12Resource* vertexResource = nullptr;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//ビューポート
	D3D12_VIEWPORT viewport{};
	//シザー矩形
	D3D12_RECT scissorRect{};

	ID3D12Resource* materialResource;
	Matrix4x4* wvpData = nullptr;
	ID3D12Resource* wvpResource;

	Transform transform;
	
	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;

	bool useMonsterBall = true;

	std::unique_ptr<ViewProjection> viewProjection_;

	//-------------
	std::unique_ptr<FogEffect>fog_;


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
	/// viewPortとscissorのセット
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void SetViewPortAndScissor(uint32_t width, uint32_t height);

	/// <summary>
	/// 描画コマンドリストの取得
	/// </summary>
	/// <returns>描画コマンドリスト</returns>
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList; }

	
	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// 三角形の描画
	/// </summary>
	void UpdatePolygon();

	/// <summary>
	/// 三角形の描画
	/// </summary>
	void DrawPolygon();

	/// <summary>
	/// 球体の描画
	/// </summary>
	void DrawSphere();


	/// <summary>
	/// ディスクリプタヒープの生成
	/// </summary>
	ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device,
											   D3D12_DESCRIPTOR_HEAP_TYPE heapType,
											   UINT numDescriptors,
											   bool shaderVisible);
	


	void Pipeline();

	
	//================
	//アクセッサ
	//================
	
	/// <summary>
	/// デバイスの取得
	/// </summary>
	ID3D12Device* GetDevice()const{ return device; }

	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc()const{ return swapChainDesc; }
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc()const{ return rtvDesc; }
	void SetCommandList(ID3D12GraphicsCommandList* comandList){ commandList = comandList; }

	uint32_t GetBufferWidth()const{ return bufferWidth_; }
	uint32_t GetBufferHeight()const{ return bufferHeight_; }

	D3D12_GRAPHICS_PIPELINE_STATE_DESC GetGraphicsPSODesc()const { return graphicsPipelineStateDesc; }

	ID3D12DescriptorHeap* GetRtvDescriptorHeap()const{ return rtvDescriptorHeap; }
	ID3D12DescriptorHeap* GetSrvDescriptorHeap()const{ return dsvDescriptorHeap; }

	uint32_t GetDescriptorSizeSRV()const{ return descriptorSizeSRV; }
	uint32_t GetDescriptorSizeRTV()const{ return descriptorSizeRTV; }
	uint32_t GetDescriptorSizeDSV()const{ return descriptorSizeDSV; }

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
	/// レンダーターゲット生成
	/// </summary>
	void CreateFinalRenderTargets();

	/// <summary>
	/// フェンス生成
	/// </summary>
	void CreateFence();

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
	/// vertexBufferViewの作成
	/// </summary>
	void CreateVertexBufferView();

	/// <summary>
	/// Resourceにデータを書き込む
	/// </summary>
	void UploadVertexData();

	//=============================================
	//		POS

	void CreateRootSignature();
	
};

