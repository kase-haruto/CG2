#include "Object3D.hlsli"

struct TransformationMatrix {
    float4x4 World;
    float4x4 WVP;
    float4x4 WorldInverseTranspose;
};

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

ConstantBuffer<TransformationMatrix> gTransformationMatirx : register(b0);
StructuredBuffer<Well> gMatrixPalette : register(t0);

Skinned Skinning(VertexShaderInput input){

    Skinned skinned;

	// ˆÊ’u‚Ì•ÏŠ·
    skinned.position = mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
    skinned.position += mul(input.position, gMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
    skinned.position += mul(input.position, gMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
    skinned.position += mul(input.position, gMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
    skinned.position.w = 1.0f;

	// –@ü‚Ì•ÏŠ·
    skinned.normal = mul(input.normal, (float3x3) gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.x;
    skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMatrix) * input.weight.y;
    skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMatrix) * input.weight.z;
    skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMatrix) * input.weight.w;
	// ³‹K‰»‚µ‚Ä–ß‚·
    skinned.normal = normalize(skinned.normal);

    return skinned;
}

VertexShaderOutput main(VertexShaderInput input){

    VertexShaderOutput output;
	
    Skinned skinned = Skinning(input);
	
	// Skinning‚ğg‚Á‚Ä•ÏŠ·
    output.position = mul(skinned.position, gTransformationMatirx.WVP);
    output.worldPosition = mul(skinned.position, gTransformationMatirx.World).xyz;
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(skinned.normal, (float3x3) gTransformationMatirx.WorldInverseTranspose));
	
    return output;
}