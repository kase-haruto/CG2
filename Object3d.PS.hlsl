#include"Object3d.hlsli"

Texture2D<float4> gTexture:register(t0);
SamplerState gSampler:register(s0);
ConstantBuffer<Material>gMaterial : register(b0);
ConstantBuffer<DirectionalLight>gDirectionalLight:register(b3);

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;

	//�e�N�X�`���J���[
	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);

	//// �t�H�O�̖��x���v�Z ���̒��ł�0���̊O�ł�1
	//float fogFactor = saturate((distanceToCamera - fogStart) / (fogEnd - fogStart));

	// ���̐F�ƃt�H�O�̐F���u�����h
	//float4 foggedColor = lerp(textureColor, fogColor, fogFactor);

	if (gMaterial.enableLighting != 0){
		float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
		output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
	} else{//���C�e�B���O���Ȃ�
		output.color = gMaterial.color * textureColor;
	}
	return output;
}