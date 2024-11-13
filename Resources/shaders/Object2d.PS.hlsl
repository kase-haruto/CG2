#include "Object2d.hlsli"

struct PSOutput
{
    float4 color : SV_TARGET0;
};

struct Material
{
    float4 color;
    float4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(VSOutput input)
{
    PSOutput output;

   // UV���W��ϊ�
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float2 clampedUV = frac(transformedUV.xy); // UV���W��0?1�͈̔͂Ɏ��߂�

    float4 textureColor = gTexture.Sample(gSampler, clampedUV);

    // �A���t�@�u�����h�̍l��
    output.color = gMaterial.color * textureColor;
    
    return output;
}