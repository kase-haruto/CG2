#pragma once
#include <unordered_map>
#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class ResourceStateTracker{
public:
    ResourceStateTracker() = default;
    ~ResourceStateTracker() = default;

    // リソースの現在の状態を取得
    D3D12_RESOURCE_STATES GetResourceState(ID3D12Resource* resource){
        auto it = resourceStates_.find(resource);
        if (it != resourceStates_.end()){
            return it->second;
        }
        // 初期状態が不明な場合は COMMON を返す
        return D3D12_RESOURCE_STATE_COMMON;
    }

    // リソースの状態を設定
    void SetResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state){
        resourceStates_[resource] = state;
    }

    // リソースを削除
    void RemoveResource(ID3D12Resource* resource){
        resourceStates_.erase(resource);
    }

private:
    // リソースとその状態をマッピングするハッシュマップ
    std::unordered_map<ID3D12Resource*, D3D12_RESOURCE_STATES> resourceStates_;
};


