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

    // スワップチェイン設定
    swapChainDesc_.Width = width;
    swapChainDesc_.Height = height;
    swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc_.SampleDesc.Count = 1;
    swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc_.BufferCount = 2;
    swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc_.Scaling = DXGI_SCALING_STRETCH;

    // スワップチェイン作成
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

    hr = tempSwapChain.As(&swapChain_);
    assert(SUCCEEDED(hr));

    // モニターのリフレッシュレートを取得
    ComPtr<IDXGIOutput> output;
    hr = swapChain_->GetContainingOutput(&output);

    if (FAILED(hr)){
        OutputDebugStringA("Failed to get containing output. Using default refresh rate.\n");
        refreshRate_ = 60.0f; // デフォルト値
    } else{
        // リフレッシュレート取得
        DXGI_MODE_DESC modeDesc = {};
        modeDesc.Width = width;
        modeDesc.Height = height;
        modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        DXGI_MODE_DESC closestMatch = {};
        hr = output->FindClosestMatchingMode(&modeDesc, &closestMatch, nullptr);

        if (FAILED(hr) || closestMatch.RefreshRate.Numerator == 0 || closestMatch.RefreshRate.Denominator == 0){
            OutputDebugStringA("Failed to get refresh rate, using default 60Hz.\n");
            refreshRate_ = 60.0f;
        } else{
            refreshRate_ = static_cast< float >(closestMatch.RefreshRate.Numerator) / closestMatch.RefreshRate.Denominator;
        }
    }

    syncInterval_ = static_cast< UINT >(std::round(refreshRate_ / 60.0f));
    if (syncInterval_ < 1) syncInterval_ = 1;

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
