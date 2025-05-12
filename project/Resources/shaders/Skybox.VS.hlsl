#include "Skybox.hlsli"

struct VertexShaderInput {
    float4 position : POSITION0;
};

cbuffer ObjectConstants : register(b0){
    float4x4 World;
    float4x4 WorldInverseTranspose;
}

VertexShaderOutput main(VertexShaderInput input){
    VertexShaderOutput output;

    
    output.position = mul(input.position, ViewProjection).xyww;

    // テクスチャ座標をそのまま出力
    output.texcoord = input.position.xyz;

    return output;
}