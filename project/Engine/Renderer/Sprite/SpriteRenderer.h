#pragma once

#include <vector>
#include <d3d12.h>

class Sprite;

class SpriteRenderer {
public:
	void Register(Sprite* sprite);
	void Draw(ID3D12GraphicsCommandList* cmdList, class PipelineService* psoService);
	void Clear();

private:
	std::vector<Sprite*> sprites_;
};