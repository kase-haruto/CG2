#include "Skybox.hlsli"



struct PixelShaderOutput{
    float4 color : SV_TARGET0;
};

TextureCube<float4> skyboxTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    // Skyboxのテクスチャをサンプリング
    output.color = output.color = skyboxTexture.SampleLevel(gSampler, input.texcoord, 0);
    return output;
}