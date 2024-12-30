#include"engine/core/DirectX/DxSwapChain.h"

#include <thread>

float DxSwapChain::deltaTime_ = 0.016f;

void DxSwapChain::Initialize(
    ComPtr<IDXGIFactory7> dxgiFactory,
    ComPtr<ID3D12CommandQueue> commandQueue,
    HWND hwnd,
    uint32_t width,
    uint32_t height
){
    HRESULT hr;

    // スワップチェインの設定
    swapChainDesc_.Width = width;
    swapChainDesc_.Height = height;
    swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc_.SampleDesc.Count = 1;
    swapChainDesc_.SampleDesc.Quality = 0;
    swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc_.BufferCount = 2;
    swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc_.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc_.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc_.Flags = 0;

    // スワップチェインの作成
    ComPtr<IDXGISwapChain1> tempSwapChain;
    hr = dxgiFactory->CreateSwapChainForHwnd(
        commandQueue.Get(),
        hwnd,
        &swapChainDesc_,
        nullptr,
        nullptr,
        &tempSwapChain
    );
    assert(SUCCEEDED(hr));

    // IDXGISwapChain4 にキャスト
    hr = tempSwapChain.As(&swapChain_);
    assert(SUCCEEDED(hr));

    // モニターのリフレッシュレートを取得
    ComPtr<IDXGIOutput> output;
    hr = swapChain_->GetContainingOutput(&output);
    assert(SUCCEEDED(hr));

    // DXGI_MODE_DESC に値を設定
    DXGI_MODE_DESC modeDesc = {};
    modeDesc.Width = width;
    modeDesc.Height = height;
    modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    modeDesc.RefreshRate.Numerator = 0; // デフォルト値（FindClosestMatchingModeで上書き）
    modeDesc.RefreshRate.Denominator = 0; // デフォルト値
    modeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    DXGI_MODE_DESC closestMatch = {};
    hr = output->FindClosestMatchingMode(&modeDesc, &closestMatch, nullptr);
    assert(SUCCEEDED(hr));

    // リフレッシュレートを計算
    refreshRate_ = static_cast< float >(closestMatch.RefreshRate.Numerator) / closestMatch.RefreshRate.Denominator;

    // syncInterval を計算 (60FPS に固定)
    syncInterval_ = static_cast< UINT >(std::round(refreshRate_ / 60.0f));
    if (syncInterval_ < 1) syncInterval_ = 1; // 下限を1に設定


    firstFrameTime_ = std::chrono::high_resolution_clock::now();
}


void DxSwapChain::Present(){
    // フレームの開始時刻を取得
    auto currentFrameTime = std::chrono::high_resolution_clock::now();

    // deltaTime を計算 (ミリ秒から秒に変換)
    deltaTime_ = std::chrono::duration<float, std::milli>(currentFrameTime - lastFrameTime_).count() / 1000.0f;

    // 現在のFPSを計算
    if (deltaTime_ > 0.0f){
        currentFPS_ = 1.0f / deltaTime_;
    }

    // 累積時間を正確に計算
    totalTime_ = std::chrono::duration<float>(currentFrameTime - firstFrameTime_).count();

    // 平均FPSを計算
    if (totalTime_ > 0.0f){
        averageFPS_ = static_cast< float >(frameCount_) / totalTime_;
    }

    // スワップチェインをPresent
    swapChain_->Present(syncInterval_, 0);

    // フレーム終了時刻を記録
    lastFrameTime_ = currentFrameTime;
    frameCount_++;
}
