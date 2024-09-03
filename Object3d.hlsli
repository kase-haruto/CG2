
struct VertexShaderOutput{
	float4 position:SV_POSITION;
	float2 texcoord:TEXCOORD0;
	float3 normal:NORMAL0;
	float3 worldPosition:POSITION0;
};

//���[���h
struct TransformationMatrix{
	float4x4 WVP;
	float4x4 world;
	float4x4 worldInverseTranspose;
};

struct VertexShaderInput{
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal:NORMAL0;
};

//�}�e���A��
struct Material{
	float4 color;			//�F
	int enableLighting;		//���C�e�B���O�̎��
	float4x4 uvTransform;	//uv���W
	float shiniess;		//����x
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
struct Camera{
	float3 worldPosition;	//�J�����̃��[���h���W
};

//���C�g
struct DirectionalLight{
	float4 color;		//���C�g�̐F
	float3 direction;	//���C�g�̌���
	float intensity;	//�P�x
};

struct PointLight{
	float4 color;		//���C�g�̐F
	float3 position;	//���C�g�̈ʒu
	float intensity;	//���x
};