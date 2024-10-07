#include "core/DirectX/RenderTarget.h"
#include "myFunc/DxFunc.h"

void RenderTarget::Initialize(ComPtr<ID3D12Device> device, ComPtr<IDXGISwapChain4> swapChain){
    // RTVヒープの作成
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 2;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap_));

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    rtvDescriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // スワップチェインのバックバッファを取得しRTVを作成
    for (UINT i = 0; i < 2; ++i){
        ComPtr<ID3D12Resource> backBuffer;
        swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
        backBuffers_[i] = backBuffer; // バックバッファを保存
        device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
        rtvHandle.ptr += rtvDescriptorSize_;
    }
}

void RenderTarget::CreateDepthBuffer(ComPtr<ID3D12Device> device, uint32_t width, uint32_t height){
    depthStencilResource_ = CreateDepthStencilTextureResource(device.Get(), width, height);

    // DSV用のヒープでディスクリプタの数は1, DSVはShader内で触るものではないのでShaderVisibleはfalse
    dsvHeap_ = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

    // DSVの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Formatは基本的にResourceに合わせる
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2D Texture

    // DSV Heapの先頭にDSVを作る
    device->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvHeap_->GetCPUDescriptorHandleForHeapStart());
}

void RenderTarget::ClearRenderTarget(ComPtr<ID3D12GraphicsCommandList> commandList, UINT backBufferIndex){
    // バックバッファの対応するRTVハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += backBufferIndex * rtvDescriptorSize_;

    // DSVヒープの先頭ハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();

    // 描画先のRTVとDSVを設定
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // クリアカラーの設定
    float clearColor[] = {0.1f, 0.25f, 0.5f, 1.0f};

    // レンダーターゲットのクリア
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // 深度バッファのクリア
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

ComPtr<ID3D12Resource> RenderTarget::CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height){
    // 生成するResourceの設定
    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Width = width; // Textureの幅
    resourceDesc.Height = height;
    resourceDesc.MipLevels = 1;
    resourceDesc.DepthOrArraySize = 1; // 奥行きor配列Textureの配列数
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

    // 深度値のClear設定
    D3D12_CLEAR_VALUE depthClearValue = {};
    depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // FormatはResourceに合わせる

    // Resourceの生成
    ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, // Heapの設定
        D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定特になし
        &resourceDesc, // Resourceの設定
        D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
        &depthClearValue, // Clear最適値
        IID_PPV_ARGS(&resource) // 作成するResourceポインタへのポインタ
    );
    assert(SUCCEEDED(hr));

    return resource.Get();
}
