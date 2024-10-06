#include "Fragment.hlsli"

//ワールド変換行列
cbuffer VSConstants : register(b0){
    TransformationMatrix gTransformationMat;
}

VSOutput main(float4 pos : POSITION, float4 color : COLOR){
    VSOutput output; // ピクセルシェーダーに渡す値
    output.svpos = mul(pos, gTransformationMat.WVP);
    output.color = color;

    return output;
}