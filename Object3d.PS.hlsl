#include"Object3d.hlsli"

Texture2D<float4> gTexture:register(t0);
SamplerState gSampler:register(s0);
ConstantBuffer<Material>gMaterial : register(b0);
ConstantBuffer<DirectionalLight>gDirectionalLight:register(b2);

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;

	//�e�N�X�`���J���[
	//float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	// UV���W��ϊ�
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);

    // �e�N�X�`���T���v�����擾
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);


	//// �t�H�O�̖��x���v�Z ���̒��ł�0���̊O�ł�1
	//float fogFactor = saturate((distanceToCamera - fogStart) / (fogEnd - fogStart));

	// ���̐F�ƃt�H�O�̐F���u�����h
	//float4 foggedColor = lerp(textureColor, fogColor, fogFactor);

    if (textureColor.a == 0.5){
        discard;
    }

    if (gMaterial.enableLighting == 0){
        // Half-Lambert shading
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float halfLambertTerm = pow(NdotL * 0.5f + 0.5f, 2.0f);
        output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * halfLambertTerm * gDirectionalLight.intensity;
        output.color.a = gMaterial.color.a * textureColor.a;

    } else if (gMaterial.enableLighting == 1){
        // Lambert shading
        float lambertTerm = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * lambertTerm * gDirectionalLight.intensity;
        output.color.a = gMaterial.color.a * textureColor.a;
    } else{
        // No lighting
        output.color = gMaterial.color * textureColor;
    }

  
    if (output.color.a == 0.0){
        discard;
    }

	return output;
}