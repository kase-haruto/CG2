#include "DirectX12.h"
#include"ConvertString.h"
#include<format>
#include<cassert>

// DirectX ライブラリのリンカー指示
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

DirectX12* DirectX12::GetInstance() {
	static DirectX12 instance;
	return &instance;
}

void DirectX12::InitializeDXGIDevice() {
	//dxgiファクトリーの生成
	
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));
	//使用するアダプタ用の変数
	IDXGIAdapter4* useAdapter = nullptr;
	//よい順にアダプタをたのむ
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		 DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		 DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));
		//ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報をログに出力。wstringのほうなので注意
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		//ソフトウェアアダプタの場合は見なかったことにする
		useAdapter = nullptr;
	}

	ID3D12Device* device = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプタでデバイスを生成
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			//生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel:{}\n", featureLevelStrings[i]));
			break;
		}
	}
	//デバイスの生成がうまくいかなかったので起動できない
	assert(device != nullptr);
	Log("Complete create D3D12Device!!!\n");// 初期化完了のログを出す
}

void DirectX12::InitializeCommand() {
	//コマンドキューを生成
	
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc,
									IID_PPV_ARGS(&commandQueue));
	//コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドアロケータを生成する
	ID3D12CommandAllocator* commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	//コマンドアロケータを生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドリストを生成する
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr,
								   IID_PPV_ARGS(&commandList));
	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

}

void DirectX12::CreateSwapChain() {
	//スワップチェーンの作成
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = WinApp::kWindowWidth;//画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Height = WinApp::kWindowHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式
	swapChainDesc.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;//ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニタに移したら中身を破棄
	
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, winApp_->GetHWND(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	assert(SUCCEEDED(hr));
}

void DirectX12::CreateDescriptorHeap() {
	//ディスクリプタヒープの生成
	ID3D12DescriptorHeap* rtvDecriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
}