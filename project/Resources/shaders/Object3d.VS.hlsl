#include "Object3d.hlsli"

//���[���h�ϊ��s��
cbuffer VSConstants : register(b0){
    TransformationMatrix gTransformationMat;
}

VertexShaderOutput main(VertexShaderInput input){
    VertexShaderOutput output;

    // ���[���h�A�r���[�A�v���W�F�N�V�����ϊ�
    output.position = mul(input.position, gTransformationMat.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMat.world));
    output.worldPosition = mul(input.position, gTransformationMat.world).xyz;

    return output;
}