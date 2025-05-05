#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include "IRenderTarget.h"

class RenderTargetCollection{
public:
	RenderTargetCollection() = default;
	~RenderTargetCollection() = default;

	void Add(const std::string& name, std::unique_ptr<IRenderTarget> target);
	IRenderTarget* Get(const std::string& name) const;
	void ClearAll(ID3D12GraphicsCommandList* cmdList);

private:
	std::unordered_map<std::string, std::unique_ptr<IRenderTarget>> targets_;
};