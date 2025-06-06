#pragma once

class IOnViewportTool{
public:
	virtual ~IOnViewportTool() = default;
	virtual void Update() = 0;
	virtual void RenderOverlay() = 0;
	virtual void RenderToolbar() = 0;
};