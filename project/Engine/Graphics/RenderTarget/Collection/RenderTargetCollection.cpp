#include "RenderTargetCollection.h"
#include <d3d12.h>

void RenderTargetCollection::Add(RenderTargetType type, const std::string& name, std::unique_ptr<IRenderTarget> target){
	targetsByType_[type] = target.get();
	targetsByName_[name] = std::move(target);
}

IRenderTarget* RenderTargetCollection::Get(const std::string& name) const{
    auto it = targetsByName_.find(name);
    return (it != targetsByName_.end()) ? it->second.get() : nullptr;
}

IRenderTarget* RenderTargetCollection::Get(RenderTargetType type) const{
    auto it = targetsByType_.find(type);
    return (it != targetsByType_.end()) ? it->second : nullptr;
}
