#include"Object3d.hlsli"

Texture2D<float4> gTexture:register(t0);
SamplerState gSampler:register(s0);

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;

	//�e�N�X�`���J���[
	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
   
	//�J��������̋���
    float distanceToCamera = length(input.worldPosition.xyz - cameraPos);
	
	// �t�H�O�̖��x���v�Z ���̒��ł�0���̊O�ł�1
	float fogFactor = saturate((distanceToCamera - fogStart) / (fogEnd - fogStart));

    // ���̐F�ƃt�H�O�̐F���u�����h
	float4 foggedColor = lerp(textureColor, fogColor, fogFactor);

	// �o�̓J���[�Ƀt�H�O��K�p
	output.color = gMaterial.color * foggedColor;

	return output;
}