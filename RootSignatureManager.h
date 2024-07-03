#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <mutex>

using namespace Microsoft::WRL;

class DirectXCommon;

/// <summary>
/// ルートシグネーチャ
/// </summary>
class RootSignatureManager{
public:
    // シングルトンインスタンスの取得
    static RootSignatureManager* GetInstance();

    // インスタンスのコピーや代入を禁止
    RootSignatureManager(const RootSignatureManager&) = delete;
    RootSignatureManager& operator=(const RootSignatureManager&) = delete;

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(DirectXCommon* directXCommon);
    /// <summary>
    /// 更新
    /// </summary>
    void Update();
    /// <summary>
    /// 生成
    /// </summary>
    void CreateRootSignature();    
    /// <summary>
    /// 生成したsignatureの受け渡し
    /// </summary>
    /// <returns></returns>
    ID3D12RootSignature* GetRootSignature()const{ return rootSignature_.Get(); }

private:
    RootSignatureManager() = default;
    ~RootSignatureManager() = default;

    //=================================
    //  DirectX関連
    //=================================
    ComPtr<ID3D12Device> device_ = nullptr;
    ComPtr<ID3DBlob> signatureBlob_ = nullptr;
    ComPtr<ID3DBlob> errorBlob_ = nullptr;
    ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
};
