#include"Particle.hlsli"

Texture2D<float4> gTexture:register(t0);
SamplerState gSampler:register(s0);
ConstantBuffer<Material>gMaterial : register(b0);

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;

	// UV���W��ϊ�
	float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);

	// �e�N�X�`���T���v�����擾
	float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	output.color = gMaterial.color * textureColor*input.color;
	//putput.color�̒l��0�̎���pixel��j��
	if (output.color.a == 0.0){
		discard;
	}
	return output;
}