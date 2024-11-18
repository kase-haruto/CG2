// ���[���h�ϊ��̍\���́FVS��PS�ŋ��L
struct TransformationMatrix{
    float4x4 WVP;                   // ���[���h�A�r���[�A�v���W�F�N�V�����ϊ�
    float4x4 world;                 // ���[���h�ϊ��s��
    float4x4 worldInverseTranspose; // �@���ϊ��p�̋t�]�u�s��
};

// Vertex Shader �̓��͗p�\����
struct VertexShaderInput{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal :   NORMAL0;
};

// Vertex Shader �̏o�͗p�\���́iPixel Shader �̓��́j
struct VertexShaderOutput{
    float4 position :      SV_POSITION;
    float2 texcoord :      TEXCOORD0;
    float3 normal :        NORMAL0;
    float3 worldPosition : POSITION0;
};
