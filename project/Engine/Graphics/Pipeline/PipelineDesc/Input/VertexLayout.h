#pragma once

#include <Engine/Foundation/Math/Vector2.h>
#include <Engine/Foundation/Math/Vector3.h>
#include <Engine/Foundation/Math/Vector4.h>

#include <d3d12.h>
#include <d3dx12.h>
#include <vector>

// 1️⃣ まずはテンプレート宣言
template<typename T>
struct VertexInputLayout;

// 2️⃣ 頂点構造体定義
struct VertexPosUv {
	Vector3 pos;
	Vector2 uv;
};

struct VertexPosColor {
	Vector3 pos;
	Vector4 color;
};

struct VertexPosUvColor {
	Vector4 pos;
	Vector2 uv;
	Vector4 color;
};

struct VertexPosUvN {
	Vector4 position;   // 16 B (レジスタ0)
	Vector2 texcoord;   //  8 B
	Vector3 normal;
};

struct VertexPosUvNSkinning {
	Vector3 pos;
	Vector2 uv;
	Vector3 normal;
	Vector4 weight;
	int index[4];
};

template<>
struct VertexInputLayout<VertexPosUvN> {
	static std::vector<D3D12_INPUT_ELEMENT_DESC> Get() {
		return {
			// Semantic   Idx  Format                          Slot Offset
			{ "POSITION", 0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			  D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

			{ "TEXCOORD", 0,  DXGI_FORMAT_R32G32_FLOAT,       0,
			  D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

			{ "NORMAL",   0,  DXGI_FORMAT_R32G32B32_FLOAT,    0,
			  D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
	}
};

template<>
struct VertexInputLayout<VertexPosUv> {
	static std::vector<D3D12_INPUT_ELEMENT_DESC> Get() {
		return {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			  D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			  D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
	}
};

template<>
struct VertexInputLayout<VertexPosUvNSkinning> {
	static std::vector<D3D12_INPUT_ELEMENT_DESC> Get() {
		return {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			  D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			  D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			  D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "WEIGHT",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			  D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "INDEX",    0, DXGI_FORMAT_R32G32B32A32_SINT, 1,
			  D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
	}
};
