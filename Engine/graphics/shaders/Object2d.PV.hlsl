#include "Object2d.hlsli"

struct PSOutput
{
    float4 color : SV_TARGET0;
};

struct Material{
    float4 color;
    float4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(VSOutput input)
{
    PSOutput output;
    
    float3 transformedUV = mul(float4(input.texcoord, 1.0f), gMaterial.uvTransform);

    // テクスチャカラーをサンプリング
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    // アルファブレンドの考慮
    output.color = gMaterial.color * textureColor;

    return output;
}
