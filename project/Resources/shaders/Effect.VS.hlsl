#include "Effect.hlsli"

/////////////////////////////////////////////////////////////////////////
//                      Material
/////////////////////////////////////////////////////////////////////////
cbuffer MATERIAL : register(b0) {
    float4 materialColor; // �}�e���A���J���[
}

/////////////////////////////////////////////////////////////////////////
//                      world/view/projection matrix
/////////////////////////////////////////////////////////////////////////
cbuffer VSConstants : register(b1) {
    TransformationMatrix gTransformationMat; // �ϊ��s��
}

/////////////////////////////////////////////////////////////////////////
//                      main
/////////////////////////////////////////////////////////////////////////
VSOutput main(float4 pos : POSITION,
                float2 texcoord : TEXCOORD,
                float4 color : COLOR)
{
    VSOutput output;

    // ���_�ʒu��ϊ� (WVP �s����g�p)
    output.svpos = mul(pos, gTransformationMat.WVP);

    // �e�N�X�`�����W�����̂܂ܓn��
    output.texcoord = texcoord;

    // �}�e���A���J���[����Z���ĐF���v�Z
    output.color = color * materialColor;

    return output;
}
