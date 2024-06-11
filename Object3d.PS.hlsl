#include"Object3d.hlsli"

Texture2D<float4> gTexture:register(t0);
SamplerState gSampler:register(s0);
ConstantBuffer<Material>gMaterial : register(b0);
ConstantBuffer<DirectionalLight>gDirectionalLight:register(b2);

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;

	//テクスチャカラー
	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);

	//// フォグの密度を計算 霧の中では0霧の外では1
	//float fogFactor = saturate((distanceToCamera - fogStart) / (fogEnd - fogStart));

	// 元の色とフォグの色をブレンド
	//float4 foggedColor = lerp(textureColor, fogColor, fogFactor);

	

	if (gMaterial.enableLighting != 0){
		//half lambert
		float NdotL = dot(normalize(input.normal),-gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f*0.5f,2.0f);
		output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
	} else{//ライティングしない
		output.color = gMaterial.color * textureColor;
	}

	return output;
}