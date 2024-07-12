#include"Object3d.hlsli"

Texture2D<float4> gTexture:register(t0);
SamplerState gSampler:register(s0);
ConstantBuffer<Material>gMaterial : register(b0);
ConstantBuffer<DirectionalLight>gDirectionalLight:register(b2);

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;

	//テクスチャカラー
	//float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	// UV座標を変換
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);

    // テクスチャサンプルを取得
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);


	//// フォグの密度を計算 霧の中では0霧の外では1
	//float fogFactor = saturate((distanceToCamera - fogStart) / (fogEnd - fogStart));

	// 元の色とフォグの色をブレンド
	//float4 foggedColor = lerp(textureColor, fogColor, fogFactor);

	//aが0.5以下の時にpixelを破棄
	if (textureColor.a <= 0.5){
		discard;
	}

	if (gMaterial.enableLighting != 0){
		//half lambert
		float NdotL = dot(normalize(input.normal),-gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f+0.5f,2.0f);
		output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
		output.color.a = gMaterial.color.a * textureColor.a;
	} else{//ライティングしない
		output.color = gMaterial.color * textureColor;
	}

	
	//putput.colorの値が0の時にpixelを破棄
	if (output.color.a == 0.0){
		discard;
	}
	//textureのa値が0の時pixelを破棄
	if (textureColor.a == 0){
		discard;
	}

	return output;
}