#include "SkinningObject3d.hlsli"

struct Well {
    float4x4 skeletonSpaceMat;
    float4x4 skeletonSpaceInverseTransposeMat;
};

struct Skinned {
    float4 position;
    float3 normal;
};

StructuredBuffer<Well> gMatrixPalette : register(t0);
ConstantBuffer<TransformationMatrix> gTransformationMatirx : register(b0);

Skinned Skinning(VertexShaderInput input){
    Skinned skinned;

    float4 pos = float4(input.position.xyz, 1.0f);

    // 位置のスキニング（skeletonSpaceMat を使用）
    skinned.position = mul(pos, gMatrixPalette[input.index.x].skeletonSpaceMat) * input.weight.x;
    skinned.position += mul(pos, gMatrixPalette[input.index.y].skeletonSpaceMat) * input.weight.y;
    skinned.position += mul(pos, gMatrixPalette[input.index.z].skeletonSpaceMat) * input.weight.z;
    skinned.position += mul(pos, gMatrixPalette[input.index.w].skeletonSpaceMat) * input.weight.w;

    // 法線のスキニング
    skinned.normal = mul(input.normal, (float3x3) gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMat) * input.weight.x;
    skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMat) * input.weight.y;
    skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMat) * input.weight.z;
    skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMat) * input.weight.w;
    skinned.normal = normalize(skinned.normal);

    return skinned;
}

VertexShaderOutput main(VertexShaderInput input){

    VertexShaderOutput output;
    Skinned skinned = Skinning(input);

    output.position = mul(skinned.position, gTransformationMatirx.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(skinned.normal, (float3x3) gTransformationMatirx.world));
    output.worldPosition = mul(skinned.position, gTransformationMatirx.world).xyz;

    return output;
}