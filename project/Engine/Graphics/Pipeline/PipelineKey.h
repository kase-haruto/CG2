#pragma once
#include <cstdint>
#include <functional>
#include "../blendMode/BlendMode.h" 
#include "PipelineType.h"

struct PipelineKey{
	PipelineType pipelineType;
	BlendMode    blendMode;

	// 比較演算子
	bool operator==(const PipelineKey& other) const{
		return (pipelineType == other.pipelineType) && (blendMode == other.blendMode);
	}
};

// std::hash の特殊化
namespace std{
	template<>
	struct hash<PipelineKey>{
		size_t operator()(const PipelineKey& key) const{
			auto pt = static_cast< size_t >(key.pipelineType);
			auto bm = static_cast< size_t >(key.blendMode);
			return (pt << 4) ^ bm;
		}
	};
}
