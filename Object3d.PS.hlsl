#include"Object3d.hlsli"

struct Material{
	float4 color;
};

ConstantBuffer<Material>gMaterial : register(b0);
Texture2D<float4> gTexture:register(t0);
SamplerState gSampler:register(s0);

struct PixelShaderOutput{
	float4 color : SV_TARGET0;
};

ConstantBuffer<FogParmeters>gFogParams:register(b1);
// �t�H�O�̃p�����[�^���`
struct FogParameters{
	float4 fogColor;//���̐F
	float fogStart;//���̊J�n����
	float fogEnd;//���̏I������
};

cbuffer CameraBuffer:register(b2){
	float3 cameraPos;
};

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;
	//�e�N�X�`���J���[
	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	output.color = gMaterial.color*textureColor;
	return output;
}