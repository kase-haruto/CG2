#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 定数バッファから中心と幅を受け取る
cbuffer BlurParmeterCB : register(b0) {
    float2 centerPos;
    float width;
}

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
    float2 texcoord : TEXCOORD0;
};

// 入力は、VertexShader から渡されたUV
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    const int kNumSamples = 10;

    // 中心方向のベクトル（方向）
    float2 direction = input.texcoord - centerPos;

    // ブラー用の加算カラー
    float3 accumulatedColor = float3(0.0f, 0.0f, 0.0f);

    // 中心から外側に向かってぼかし
    for (int i = 0; i < kNumSamples; i++)
    {
        float2 offsetTexcoord = input.texcoord + direction * width * float(i);
        accumulatedColor += gTexture.Sample(gSampler, offsetTexcoord).rgb;
    }

    // 平均化してブラー色に
    accumulatedColor *= rcp(float(kNumSamples));

    output.color = float4(accumulatedColor, 1.0f);
    output.texcoord = input.texcoord;
    return output;
}
