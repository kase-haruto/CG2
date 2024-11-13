#pragma pack_matrix(row_major)

// ���[���h�ϊ��̍\���́FVS��PS�ŋ��L
struct TransformationMatrix{
    float4x4 WVP; // ���[���h�A�r���[�A�v���W�F�N�V�����ϊ�
    float4x4 world; // ���[���h�ϊ��s��
    float4x4 worldInverseTranspose; // �@���ϊ��p�̋t�]�u�s��
};

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput{
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
    float4 color : COLOR; // �F(RGBA)
};
