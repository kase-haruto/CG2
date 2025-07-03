#pragma once

/* ========================================================================
/*	include	space
/* ===================================================================== */

// enigne
#include <Engine/Graphics/RenderTarget/Interface/IRenderTarget.h>
#include <Engine/Graphics/RenderTarget/RenderTargetDetails.h>

// c++
#include <string>
#include <unordered_map>
#include <memory>

class RenderTargetCollection{
public:
    void Add(RenderTargetType type, const std::string& name, std::unique_ptr<IRenderTarget> target);

    IRenderTarget* Get(const std::string& name) const;
    IRenderTarget* Get(RenderTargetType type) const;

    void ClearAll(ID3D12GraphicsCommandList* cmdList);

private:
    std::unordered_map<std::string, std::unique_ptr<IRenderTarget>> targetsByName_;
    std::unordered_map<RenderTargetType, IRenderTarget*> targetsByType_;
};