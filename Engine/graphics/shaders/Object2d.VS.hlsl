#include "Object2d.hlsli"

struct VSInput{
    float4 position : POSITION; // POSITIONをfloat2で定義
    float2 texcoord : TEXCOORD0;
};

//ワールド変換行列
cbuffer VSConstants : register(b0){
    TransformationMatrix gTransformationMat;
}

VSOutput main(VSInput input){
    VSOutput output;
    
    output.position = mul(input.position, gTransformationMat.WVP);
    output.texcoord = input.texcoord;

    return output;
}
