#include"NonTextureObject.hlsli"

Texture2D<float4> gTexture:register(t0);
SamplerState gSampler:register(s0);
ConstantBuffer<Material>gMaterial : register(b0);
ConstantBuffer<DirectionalLight>gDirectionalLight:register(b2);

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;

	if (gMaterial.enableLighting == 0){
		//half lambert
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		output.color.rgb = gMaterial.color.rgb  * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
		output.color.a = gMaterial.color.a;
	} else if (gMaterial.enableLighting == 1){
        // Lambert shading
        float lambertTerm = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        output.color = gMaterial.color* gDirectionalLight.color * lambertTerm * gDirectionalLight.intensity;

    } else{
        // No lighting
        output.color = gMaterial.color;
    }


	return output;
}