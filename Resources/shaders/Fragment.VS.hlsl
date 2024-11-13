#include "Fragment.hlsli"

//���[���h�ϊ��s��
cbuffer VSConstants : register(b0){
    TransformationMatrix gTransformationMat;
}

VSOutput main(float4 pos : POSITION, float4 color : COLOR){
    VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
    output.svpos = mul(pos, gTransformationMat.WVP);
    output.color = color;

    return output;
}