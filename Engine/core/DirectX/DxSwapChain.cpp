#include"core/DirectX/DxSwapChain.h"

void DxSwapChain::Initialize(ComPtr<IDXGIFactory7> dxgiFactory, ComPtr<ID3D12CommandQueue> commandQueue, HWND hwnd, uint32_t width, uint32_t height){
	HRESULT hr;

	//スワップチェーンの作成
	swapChainDesc_.Width = width;//画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Height = height;
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式
	swapChainDesc_.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画のターゲットとして利用する
	swapChainDesc_.BufferCount = 2;//ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニタに移したら中身を破棄

	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc_, nullptr, nullptr, reinterpret_cast< IDXGISwapChain1** >(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));
}

void DxSwapChain::Presetn(){
	swapChain_->Present(1,0);
}
