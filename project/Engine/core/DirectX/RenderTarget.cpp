#include "engine/core/DirectX/RenderTarget.h"
#include "lib/myFunc/DxFunc.h"
#include "Engine/graphics/SrvLocator.h"

void RenderTarget::Initialize(ComPtr<ID3D12Device> device, DxSwapChain& swapChain){
    HRESULT hr;
    // ディスクリプタの生成
    rtvHeap_ = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

    // rtvDescriptorSize_の初期化
    rtvDescriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // swapChainからResourceを取得
    ComPtr<ID3D12Resource> backBuffer0;
    ComPtr<ID3D12Resource> backBuffer1;
    hr = swapChain.GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(&backBuffer0));
    assert(SUCCEEDED(hr));

    hr = swapChain.GetSwapChain()->GetBuffer(1, IID_PPV_ARGS(&backBuffer1));
    assert(SUCCEEDED(hr));

    backBuffer0->SetName(L"backBuffer0");
    backBuffer1->SetName(L"backBuffer1");

    // swapChain のバックバッファに保存
    swapChain.SetBackBuffer(0, backBuffer0);
    swapChain.SetBackBuffer(1, backBuffer1);

    // rtvの設定
    rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    // ディスクリプタの先頭を取得
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();

    // rtvを2つ作成するためディスクリプタを2つ用意
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
    rtvHandles[0] = rtvStartHandle;
    device->CreateRenderTargetView(swapChain.GetBackBuffer(0).Get(), &rtvDesc_, rtvHandles[0]);

    rtvHandles[1].ptr = rtvHandles[0].ptr + rtvDescriptorSize_;
    device->CreateRenderTargetView(swapChain.GetBackBuffer(1).Get(), &rtvDesc_, rtvHandles[1]);
}

void RenderTarget::CreateDepthBuffer(ComPtr<ID3D12Device> device, uint32_t width, uint32_t height){
    depthStencilResource_ = CreateDepthStencilTextureResource(device.Get(), width, height);

    // DSV用のヒープを生成
    dsvHeap_ = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

    // DSVの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    // DSV Heapの先頭にDSVを作成
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
    float clearColor[] = {0.02f, 0.02f, 0.02f, 1.0f};


    // レンダーターゲットのクリア
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // 深度バッファのクリア
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}


ComPtr<ID3D12Resource> RenderTarget::CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height){
    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Width = width;
    resourceDesc.Height = height;
    resourceDesc.MipLevels = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_CLEAR_VALUE depthClearValue = {};
    depthClearValue.DepthStencil.Depth = 1.0f;
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthClearValue,
        IID_PPV_ARGS(&resource)
    );
    assert(SUCCEEDED(hr));

    return resource;
}

void RenderTarget::CreateOffscreenRenderTarget(ComPtr<ID3D12Device> device, uint32_t width, uint32_t height){
    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Width = width;
    resourceDesc.Height = height;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = resourceDesc.Format;
    clearValue.Color[0] = 1.0f;
    clearValue.Color[1] = 0.0f;
    clearValue.Color[2] = 0.0f;
    clearValue.Color[3] = 1.0f;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue,
        IID_PPV_ARGS(&offscreenRenderTarget_)
    );
    assert(SUCCEEDED(hr));

    offscreenRenderTarget_->SetName(L"offscreen");

    // RTV 用のディスクリプタヒープを作成
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 1; // オフスクリーンレンダーターゲット用
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&offscreenRtvHeap_));
    assert(SUCCEEDED(hr));

    // RTV を作成
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = offscreenRtvHeap_->GetCPUDescriptorHandleForHeapStart();
    device->CreateRenderTargetView(offscreenRenderTarget_.Get(), nullptr, rtvHandle);

    // SRV を作成
    CreateOffscreenSRV(device);
}

void RenderTarget::CreateOffscreenSRV(ComPtr<ID3D12Device> device){
    auto [cpuHandle, gpuHandle] = SrvLocator::AllocateSrv();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    device->CreateShaderResourceView(offscreenRenderTarget_.Get(), &srvDesc, cpuHandle);

    offscreenSrvGpuDescriptorHandle_ = gpuHandle;
}
