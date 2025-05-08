#include "Object3D.hlsli"

/*===========================================================
                     Object3D VS Shader
===========================================================*/

struct VertexShaderInput {
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

cbuffer ObjectConstants : register(b0) {
    float4x4 World;
    float4x4 WorldInverseTranspose;
}

VertexShaderOutput main(VertexShaderInput input){
    VertexShaderOutput output;

    float4 worldPos = mul(input.position, World);
    output.position = mul(worldPos, ViewProjection);
    output.worldPosition = worldPos.xyz;

    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) WorldInverseTranspose)); // 法線変換

    return output;
}