
struct VertexShaderOutput{
	float4 position:SV_POSITION;
	float2 texcoord:TEXCOORD0;
	float3 normal:NORMAL0;
};

//���[���h
struct TransformationMatrix{
	float4x4 WVP;
	float4x4 world;
};

struct VertexShaderInput{
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal:NORMAL0;
};

//�}�e���A��
struct Material{
	float4 color;
	int enableLighting;
};

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
struct CameraConstants{
	float4x4 view;			//���[���h���r���[�ϊ��s��
	float4x4 projection;	//�r���[���v���W�F�N�V�����ϊ��s��
	float3 cameraPos;		//�J�������W(���[���h���W)
};

//���C�g
struct DirectionalLight{
	float4 color;
	float3 direction;
	float intensity;
};