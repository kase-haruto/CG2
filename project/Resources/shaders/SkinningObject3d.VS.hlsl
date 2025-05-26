#include "Object3D.hlsli"

struct Well {
	float4x4 skeletonSpaceMatrix;
	float4x4 skeletonSpaceInverseTransposeMatrix;
};

struct Skinned {
	float4 position;
	float3 normal;
};

struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 weight : WEIGHT0;
	int4 index : INDEX0;
};

cbuffer ObjectConstants : register(b0) {
	float4x4 World;
	float4x4 WorldInverseTranspose;
}

StructuredBuffer<Well> gMatrixPalette : register(t0);

Skinned Skinning(VertexShaderInput input) {

	Skinned skinned;

	// 位置の変換
	skinned.position = mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
	skinned.position += mul(input.position, gMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
	skinned.position += mul(input.position, gMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
	skinned.position += mul(input.position, gMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
	skinned.position.w = 1.0f;

	// 法線の変換
	skinned.normal = mul(input.normal, (float3x3)gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.x;
	skinned.normal += mul(input.normal, (float3x3)gMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMatrix) * input.weight.y;
	skinned.normal += mul(input.normal, (float3x3)gMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMatrix) * input.weight.z;
	skinned.normal += mul(input.normal, (float3x3)gMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMatrix) * input.weight.w;
	// 正規化して戻す
	skinned.normal = normalize(skinned.normal);

	return skinned;
}

VertexShaderOutput main(VertexShaderInput input) {

	VertexShaderOutput output;
	
	Skinned skinned = Skinning(input);
	
	// Skinningを使って変換
	float4 worldPos = mul(skinned.position, World);
	output.position = mul(worldPos, ViewProjection);
	output.worldPosition = worldPos.xyz;
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(skinned.normal, (float3x3)WorldInverseTranspose));
	
	return output;
}