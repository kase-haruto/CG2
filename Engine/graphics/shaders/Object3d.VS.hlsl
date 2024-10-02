#include "Object3d.hlsli"

//ワールド変換行列
cbuffer VSConstants : register(b0){
    TransformationMatrix gTransformationMat;
}

VertexShaderOutput main(VertexShaderInput input){
    VertexShaderOutput output;

    // ワールド、ビュー、プロジェクション変換
    output.position = mul(input.position, gTransformationMat.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMat.world));
    output.worldPosition = mul(input.position, gTransformationMat.world).xyz;

    return output;
}