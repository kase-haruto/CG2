﻿#include "DirectXCommon.h"
#include"ConvertString.h"
#include<format>
#include<cassert>
#include <dxgidebug.h>
#include"Vector4.h"
#include"TextureManager.h"
#include"FogEffect.h"
#include"MyFunc.h"
#include"cmath"
#include"imgui.h"

#define _USE_MATH_DEFINES


// DirectX ライブラリのリンカー指示
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

DirectXCommon* DirectXCommon::GetInstance(){
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::Initialize(
	WinApp* win, uint32_t width, uint32_t height){
	// nullptrチェック
	assert(win);
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	winApp_ = win;
	bufferWidth_ = width;
	bufferHeight_ = height;

	SetViewPortAndScissor(width, height);

	// DXGIデバイス初期化
	InitializeDXGIDevice();

	// コマンド関連初期化
	InitializeCommand();

	// スワップチェーンの生成
	CreateSwapChain();

	// レンダーターゲット生成
	CreateFinalRenderTargets();

	//深度リソースを生成
	CreateDepthBuffer();

	//フェンスの作成
	CreateFence();

	fog_ = std::make_unique<FogEffect>(this);


	//三角形のtransform変数を作る
	transform = {{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{0.0f,0.0f,0.0f}};

	viewProjection_ = std::make_unique<ViewProjection>(this);
	//viewProjection_->Initialize();

	// ディスクリプタサイズの取得
	descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void DirectXCommon::InitializeDXGIDevice(){
#ifdef _DEBUG

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))){
		//デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG


	//dxgiファクトリーの生成
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));
	//よい順にアダプタをたのむ
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		 DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		 DXGI_ERROR_NOT_FOUND; ++i){
		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc {};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));
		//ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)){
			//採用したアダプタの情報をログに出力。wstringのほうなので注意
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		//ソフトウェアアダプタの場合は見なかったことにする
		useAdapter = nullptr;
	}

	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = {"12.2","12.1","12.0"};
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i){
		//採用したアダプタでデバイスを生成
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)){
			//生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel:{}\n", featureLevelStrings[i]));
			break;
		}
	}
	//デバイスの生成がうまくいかなかったので起動できない
	assert(device != nullptr);
	Log("Complete create D3D12Device!!!\n");// 初期化完了のログを出す

#ifdef _DEBUG
	infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))){
		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//抑制するメッセージのid
		D3D12_MESSAGE_ID denyIds[] = {
			//windows11のdxgiデバッグレイヤーとdx12デバッグレイヤーの相互作用バグによるエラーメッセージ
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = {D3D12_MESSAGE_SEVERITY_INFO};
		D3D12_INFO_QUEUE_FILTER filter {};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定s多メッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif // _DEBUG


}

void DirectXCommon::InitializeCommand(){
	//コマンドキューを生成

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc {};
	hr = device->CreateCommandQueue(&commandQueueDesc,
									IID_PPV_ARGS(&commandQueue));
	//コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドアロケータを生成する
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	//コマンドアロケータを生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドリストを生成する
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr,
								   IID_PPV_ARGS(&commandList));
	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

}

void DirectXCommon::CreateSwapChain(){
	//スワップチェーンの作成

	swapChainDesc.Width = WinApp::kWindowWidth;//画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Height = WinApp::kWindowHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式
	swapChainDesc.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;//ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニタに移したら中身を破棄

	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, winApp_->GetHWND(), &swapChainDesc, nullptr, nullptr, reinterpret_cast< IDXGISwapChain1** >(&swapChain));
	assert(SUCCEEDED(hr));
}


ID3D12DescriptorHeap* DirectXCommon::CreateDescriptorHeap(ID3D12Device* device,
														  D3D12_DESCRIPTOR_HEAP_TYPE heapType,
														  UINT numDescriptors,
														  bool shaderVisible){
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc {};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

void DirectXCommon::CreateFinalRenderTargets(){
	//ディスクリプタの生成
	rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	//swapChainからResourceを引っ張ってくる
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	//うまくできなければ起動できない
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));

	//rtvの設定
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をsrgbに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dテクスチャとして書き込む
	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//rtvを2筒来るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//まず1つ目
	rtvHandles[0] = rtvStartHandle;
	device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
	//2つ目
	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);

}

ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height){
	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc {};
	resourceDesc.Width = width;//textureの幅
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;//奥行きor配列textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencilとして使う通知

	//利用するheapの設定
	D3D12_HEAP_PROPERTIES heapProperties {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上に作る

	//深度値のClear設定
	D3D12_CLEAR_VALUE depthClearValue {};
	depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//フォーマット。Resourceと合わせる

	//Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,//heapの設定
		D3D12_HEAP_FLAG_NONE,//heapの特殊な設定特になし。
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値を書き込む状態にしておく
		&depthClearValue,//clear最適値
		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	return resource;
}

void DirectXCommon::CreateDepthBuffer(){
	depthStencilResource = CreateDepthStencilTextureResource(device, bufferWidth_, bufferHeight_);

	//DSV用のヒープでディスクリプタの数は1,DSVはShader内で触るものではないのでShaderVisibleはfalse
	dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc {};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//format基本的にはresourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
	//DSVHeapの先頭にdsvを作る
	device->CreateDepthStencilView(depthStencilResource,
								   &dsvDesc,
								   dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void DirectXCommon::CreateFence(){
	//初期値0でFenceを作る
	fenceValue = 0;
	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	//Fenceのsignalを待つためのイベントを作成する
	fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);
}

void DirectXCommon::InitializeDXC(){
	//dxcCompilerを初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));
}

IDxcBlob* DirectXCommon::CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcompiler,
	IDxcIncludeHandler* includeHandler){

	//==============================
	//HLSLファイルの読み込み
	//==============================
	//これからシェーダをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader,path:{},profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなければ止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	//UTF8の文字コードであることを通知
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;


	//==============================
	//Compileする
	//==============================
	LPCWSTR arguments[] = {
		filePath.c_str(),//コンパイル対象のhlslファイル名
		L"-E",L"main",//エントリーポイントの指定。基本的にmain以外には市内
		L"-T",profile,//ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",//デバッグ用の情報を埋め込む
		L"-Od",//最適化を外しておく
		L"-Zpr",//メモリレイアウトは行優先
	};

	IDxcResult* shaderResult = nullptr;
	//実際にshaderをコンパイルする
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,//読み込んだオプション
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		includeHandler,//includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	//==============================
	//警告、エラーが出ていないか確認
	//==============================
	// 警告、エラーが出ていないか確認
	IDxcBlobUtf8* shaderError = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);

	if (SUCCEEDED(hr) && shaderError != nullptr && shaderError->GetStringLength() != 0){
		Log(shaderError->GetStringPointer());
		assert(false); // エラーがあった場合は止める
	}


	//==============================
	//Compile結果を受け取る
	//==============================

	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeded,path:{},profile:{}\n", filePath, profile)));
	//もう使わないリソースを開放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリ返却
	return shaderBlob;
}

void DirectXCommon::CreateVertexBufferView(){
	////リソースの先頭のアドレスから使う
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	////使用するリソースのサイズは頂点3つ分のサイズ
	////vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//vertexBufferView.SizeInBytes = sizeof(VertexData) * 1536;// 16 * 16 * 6;
	////1頂点当たりのサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);

	//モデル用
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);
}

void DirectXCommon::UploadVertexData(){
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr,
						reinterpret_cast< void** >(&vertexData));
	//モデル用
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

	////左下
	//vertexData[0].position = {-0.5f,-0.5f,0.0f,1.0f};
	//vertexData[0].texcoord = {0.0f,1.0f};
	////上
	//vertexData[1].position = {0.0f,0.5f,0.0f,1.0f};
	//vertexData[1].texcoord = {0.5f,0.0f};
	////右下
	//vertexData[2].position = {0.5f,-0.5f,0.0f,1.0f};
	//vertexData[2].texcoord = {1.0f,1.0f};

	////左下２
	//vertexData[3].position = {-0.5f,-0.5f,0.5f,1.0f};
	//vertexData[3].texcoord = {0.0f,1.0f};
	////上２
	//vertexData[4].position = {0.0f,0.0f,0.0f,1.0f};
	//vertexData[4].texcoord = {0.5f,0.0f};
	////右下２
	//vertexData[5].position = {0.5f,-0.5f,-0.5f,1.0f};
	//vertexData[5].texcoord = {1.0f,1.0f};


	//// 分割数
	//const uint32_t kSubdivision = 16; // 任意の適切な値を設定
	//// 軽度分割1つ分の角度
	//const float kLonEvery = 2 * XM_PI / kSubdivision;
	//// 緯度分割1つ分の角度
	//const float kLatEvery = XM_PI / kSubdivision;

	//// 緯度の方向に分割 -n/2 ~ n/2
	//for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex){
	//	float lat = -XM_PI / 2.0f + kLatEvery * latIndex;
	//	// 経度の方向に分割 0~2π
	//	for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex){
	//		uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
	//		float lon = lonIndex * kLonEvery;

	//		//uv座標の計算
	//		float u0 = float(lonIndex) / float(kSubdivision);
	//		float u1 = float(lonIndex + 1) / float(kSubdivision);
	//		float v0 = 1.0f - float(latIndex) / float(kSubdivision);
	//		float v1 = 1.0f - float(latIndex + 1) / float(kSubdivision);

	//		//===========================================================
	//		//	三角形二枚重ねて面を作る
	//		//===========================================================

	//		//-----------------------------------------------------------
	//		//1枚目-------------------------------------------------------

	//		// a
	//		vertexData[start].position.x = std::cos(lat) * std::cos(lon);
	//		vertexData[start].position.y = std::sin(lat);
	//		vertexData[start].position.z = std::cos(lat) * std::sin(lon);
	//		vertexData[start].position.w = 1.0f;
	//		vertexData[start].texcoord = {u0, v0};
	//		vertexData[start].normal.x = vertexData[start].position.x;
	//		vertexData[start].normal.y = vertexData[start].position.y;
	//		vertexData[start].normal.z = vertexData[start].position.z;

	//		// b
	//		vertexData[start + 1].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
	//		vertexData[start + 1].position.y = std::sin(lat + kLatEvery);
	//		vertexData[start + 1].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
	//		vertexData[start + 1].position.w = 1.0f;
	//		vertexData[start + 1].texcoord = {u0, v1};
	//		vertexData[start + 1].normal.x = vertexData[start + 1].position.x;
	//		vertexData[start + 1].normal.y = vertexData[start + 1].position.y;
	//		vertexData[start + 1].normal.z = vertexData[start + 1].position.z;

	//		// c
	//		vertexData[start + 2].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
	//		vertexData[start + 2].position.y = std::sin(lat);
	//		vertexData[start + 2].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
	//		vertexData[start + 2].position.w = 1.0f;
	//		vertexData[start + 2].texcoord = {u1, v0};
	//		vertexData[start + 2].normal.x = vertexData[start + 2].position.x;
	//		vertexData[start + 2].normal.y = vertexData[start + 2].position.y;
	//		vertexData[start + 2].normal.z = vertexData[start + 2].position.z;

	//		//-----------------------------------------------------------


	//		//-----------------------------------------------------------
	//		//2枚目-------------------------------------------------------

	//		// c2
	//		vertexData[start + 3].position = vertexData[start + 2].position;
	//		vertexData[start + 3].texcoord = {u1, v0};
	//		vertexData[start + 3].normal.x = vertexData[start + 3].position.x;
	//		vertexData[start + 3].normal.y = vertexData[start + 3].position.y;
	//		vertexData[start + 3].normal.z = vertexData[start + 3].position.z;

	//		// b2
	//		vertexData[start + 4].position = vertexData[start + 1].position;
	//		vertexData[start + 4].texcoord = {u0, v1};
	//		vertexData[start + 4].normal.x = vertexData[start + 4].position.x;
	//		vertexData[start + 4].normal.y = vertexData[start + 4].position.y;
	//		vertexData[start + 4].normal.z = vertexData[start + 4].position.z;

	//		// d
	//		vertexData[start + 5].position.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
	//		vertexData[start + 5].position.y = std::sin(lat + kLatEvery);
	//		vertexData[start + 5].position.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
	//		vertexData[start + 5].position.w = 1.0f;
	//		vertexData[start + 5].texcoord = {u1, v1};
	//		vertexData[start + 5].normal.x = vertexData[start + 5].position.x;
	//		vertexData[start + 5].normal.y = vertexData[start + 5].position.y;
	//		vertexData[start + 5].normal.z = vertexData[start + 5].position.z;

	//		//------------------------------------------------------------

	//		//// 緯度の方向に分割 -n/2 ~ n/2
	//		//for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex){
	//		//	// 経度の方向に分割 0~2π
	//		//	for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex){
	//		//		uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;

	//		//		indexData[start] = start;
	//		//		indexData[start + 1] = start + 1;
	//		//		indexData[start + 2] = start + 2;
	//		//		indexData[start + 3] = start + 1;
	//		//		indexData[start + 4] = start + 3;
	//		//		indexData[start + 5] = start + 2;
	//		//	}
	//		//}
	//	}
	//}
	

}


void DirectXCommon::CreateRootSignature(){
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature {};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//srvを使用する
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算

	//RootParamer作成
	D3D12_ROOT_PARAMETER rootParamenters[5] = {};

	//定数バッファをピクセルシェーダで使用
	rootParamenters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  //cvbを使う
	rootParamenters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParamenters[0].Descriptor.ShaderRegister = 0;

	//定数バッファをバーテックスシェーダで使用
	rootParamenters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;  //cvbを使う
	rootParamenters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//vertexShaderで使う
	rootParamenters[1].Descriptor.ShaderRegister = 0;

	//フォグ定数バッファをピクセルシェーダで使用
	rootParamenters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParamenters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParamenters[2].Descriptor.ShaderRegister = 1;

	rootParamenters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//descriptorTableを使う
	rootParamenters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParamenters[3].DescriptorTable.pDescriptorRanges = descriptorRange;//tableの中身の配列を指定
	rootParamenters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

	//ライト
	rootParamenters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParamenters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParamenters[4].Descriptor.ShaderRegister = 2;

	descriptionRootSignature.pParameters = rootParamenters;
	descriptionRootSignature.NumParameters = _countof(rootParamenters);


	//samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外を利ぴ＝と
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//pixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//シリアライズしてバイナリする
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
									 D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)){
		Log(reinterpret_cast< char* >(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリをもとに生成
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
									 signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
}

void DirectXCommon::Pipeline(){
	//dxcの初期化
	InitializeDXC();
	CreateRootSignature();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc {};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc {};


	// normal blend
	/*blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;*/

	// none blend
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasterizeState
	D3D12_RASTERIZER_DESC rasterizeDesc {};
	//表面を(時計回り)表示しない
	rasterizeDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizeDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//shaderをコンパイルする
	vertexShaderBlob = CompileShader(L"Object3d.VS.hlsl",
									 L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);

	pixelShaderBlob = CompileShader(L"Object3d.PS.hlsl",
									L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqualつまり近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	//PSOを作成
	graphicsPipelineStateDesc.pRootSignature = rootSignature;
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	//vertexShader
	graphicsPipelineStateDesc.VS = {vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize()};
	//PixelShader
	graphicsPipelineStateDesc.PS = {pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize()};
	//blendState
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizeDesc;
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ｛形状｝のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
											 IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

	modelData = LoadObjFile("Resources", "plane.obj");
	vertexResource = CreateBufferResource(device, sizeof(VertexData) * modelData.vertices.size());

	materialResource = CreateBufferResource(device, sizeof(Material));


	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast< void** >(&materialData));
	//今回はあかをかきこむ
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = false;
	materialData->uvTransform = Matrix4x4::MakeIdentity();

	//WVP用のリソースを作る。
	wvpResource = CreateBufferResource(device, sizeof(TransformationMatrix));
	//データを書き込む
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast< void** >(&matrixData_));
	//単位行列を書き込んでおく
	matrixData_->WVP = Matrix4x4::MakeIdentity();


	CreateVertexBufferView();
	UploadVertexData();
}

void DirectXCommon::ClearRenderTarget(){

}

void DirectXCommon::PreDraw(){
	//rtvの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をsrgbに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dテクスチャとして書き込む
	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//rtvを2筒来るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//まず1つ目
	rtvHandles[0] = rtvStartHandle;
	device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
	//2つ目
	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();


	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier {};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース、現行のバッグバッファに対して行う
	barrier.Transition.pResource = swapChainResources[backBufferIndex];
	//遷移前(現行)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//transitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

	//描画先のrtvを設定する
	commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
	commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);
	//指定した色で画面全体をクリアする
	//float clearColor[] = {1.0f,1.0f,1.0f,1.0f};
	float clearColor[] = {0.1f,0.25f,0.5f,1.0f};
	commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);
	//指定した深度で画面全体をクリアする
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
}

void DirectXCommon::PostDraw(){
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier {};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース、現行のバッグバッファに対して行う
	barrier.Transition.pResource = swapChainResources[backBufferIndex];
	//遷移前(現行)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//transitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

	hr = commandList->Close();
	assert(SUCCEEDED(hr));

	//コマンドリストの実行
	ID3D12CommandList* commandLists[] = {commandList};
	commandQueue->ExecuteCommandLists(1, commandLists);


	//gpuとosに画面の交換を行うように通知
	swapChain->Present(1, 0);
	//Fenceの値を更新
	fenceValue++;
	//GPUがここまでたどり着いたときに,Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue->Signal(fence, fenceValue);

	//fenceの値が指定したsignalの値にたどり着いてるか確認する
	//GetCompletedValueの初期値はfence作成時に渡した初期値
	if (fence->GetCompletedValue() < fenceValue){
		//指定したsignalにたどり着いていないので、たどり着くまで待つようにイベントを指定する
		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		//イベントを待つ
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	//次のフレーム用のコマンドリストを準備
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));
}

void DirectXCommon::SetViewPortAndScissor(uint32_t width, uint32_t height){

	//クライアント領域サイズと一緒にして画面に表示
	viewport.Width = float(width);
	viewport.Height = float(height);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;


	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = width;
	scissorRect.top = 0;
	scissorRect.bottom = height;
}

void DirectXCommon::UpdatePolygon(){

#ifdef _DEBUG
	ImGui::Begin("shpere");
	ImGui::DragFloat3("translation", &transform.translate.x, 0.01f);
	ImGui::DragFloat3("rotation", &transform.rotate.x, 0.01f);
	ImGui::ColorEdit4("color", &RGBa.x);
	ImGui::Checkbox("useMonsterBall", &useMonsterBall);
	bool enableLighting = (materialData->enableLighting != 0);
	ImGui::Checkbox("enableLighting", &enableLighting);
	materialData->enableLighting = enableLighting ? 1 : 0;
	ImGui::End();
#endif // _DEBUG

	materialData->color = Vector4 (RGBa.x,RGBa.y,RGBa.z,RGBa.w);

	//transform.rotate.y += 0.03f;

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale,
											 transform.rotate,
											 transform.translate
	);
	Matrix4x4 worldViewProjectionMatrix = Matrix4x4::Multiply(worldMatrix, viewProjection_->GetViewProjection());
	matrixData_->world = worldMatrix;
	matrixData_->WVP = worldViewProjectionMatrix;
}

void DirectXCommon::DrawPolygon(){
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	//rootSignatureを設定psoに設定しているけど別途設定が必要
	commandList->SetGraphicsRootSignature(rootSignature);
	commandList->SetPipelineState(graphicsPipelineState);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	//形状を設定。psoに設定しているものとはまた別。同じものを設定すると考える
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	//フォグ用のCBufferの設定
	commandList->SetGraphicsRootConstantBufferView(2, fog_->GetConstantBuffer()->GetGPUVirtualAddress());
	//srvのdescriptorTableの先頭を設定。4はrootParamenter[4]
	commandList->SetGraphicsRootDescriptorTable(3, TextureManager::GetInstance()->GetTextureSrvHandle());
	//描画　3頂点で1つのインスタンス
	commandList->DrawInstanced(6, 1, 0, 0);
}

void DirectXCommon::DrawSphere(){
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	commandList->SetGraphicsRootSignature(rootSignature);
	commandList->SetPipelineState(graphicsPipelineState);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	//形状を設定。psoに設定しているものとはまた別。同じものを設定すると考える
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	//フォグ用のCBufferの設定
	commandList->SetGraphicsRootConstantBufferView(2, fog_->GetConstantBuffer()->GetGPUVirtualAddress());
	//srvのdescriptorTableの先頭を設定。3はrootParamenter[3]
	commandList->SetGraphicsRootDescriptorTable(3,
												useMonsterBall ?
												TextureManager::GetInstance()->GetTextureSrvHandle2() :
												TextureManager::GetInstance()->GetTextureSrvHandle());
	//描画　3頂点で1つのインスタンス
	//commandList->DrawInstanced(1536, 1, 0, 0);
	//モデル
	commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}

void DirectXCommon::Finalize(){

	CloseHandle(fenceEvent);
	fence->Release();
	rtvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();
#ifdef _DEBUG
	debugController->Release();
#endif // _DEBUG
	CloseWindow(winApp_->GetHWND());

	vertexResource->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob != nullptr){
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();

	includeHandler->Release();
	dxcCompiler->Release();
	dxcUtils->Release();

	materialResource->Release();
	wvpResource->Release();
	dsvDescriptorHeap->Release();
	depthStencilResource->Release();

	//リソースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))){
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}

	//警告時に止まる
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
}
