
struct VertexShaderOutput{
	float4 position:SV_POSITION;
	float4 worldPosition:POSITION;//���[���h���W
	float2 texcoord:TEXCOORD;
};

//���[���h
cbuffer TransformationMatrix:register(b0){
	float4x4 world;

};

struct VertexShaderInput{
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};

//�}�e���A��
struct Material{
	float4 color;
};
ConstantBuffer<Material>gMaterial : register(b0);

struct PixelShaderOutput{
	float4 color : SV_TARGET0;
};

// �t�H�O�̃p�����[�^���`
cbuffer FogConstants : register(b1){
	float fogStart;   // ���̊J�n����
	float fogEnd;     // ���̏I������
	float4 fogColor;  // ���̐F

};

//�J�����̃p�����[�^
cbuffer CameraConstants : register(b2){
	float4x4 view;			//���[���h���r���[�ϊ��s��
	float4x4 projection;	//�r���[���v���W�F�N�V�����ϊ��s��
	float3 cameraPos;		//�J�������W(���[���h���W)
};