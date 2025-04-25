#include "Effect.hlsli"
/////////////////////////////////////////////////////////////////////////
//                      world/view/projection matrix
/////////////////////////////////////////////////////////////////////////
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

/////////////////////////////////////////////////////////////////////////
//                      main
/////////////////////////////////////////////////////////////////////////
float4 main(VSOutput input) : SV_TARGET
{
    float4 texColor = gTexture.Sample(gSampler, input.texcoord);

    // �A���t�@�`�����l���𐳂����K�p����
    float alpha = texColor.a * input.color.a;

    return float4(texColor.rgb * input.color.rgb, alpha);
}