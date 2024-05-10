#include"Object3d.hlsli"

Texture2D<float4> gTexture:register(t0);
SamplerState gSampler:register(s0);

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;

	//テクスチャカラー
	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
   
	//カメラからの距離
    float distanceToCamera = length(input.worldPosition.xyz - cameraPos);
	
	// フォグの密度を計算 霧の中では0霧の外では1
	float fogFactor = saturate((distanceToCamera - fogStart) / (fogEnd - fogStart));

    // 元の色とフォグの色をブレンド
	float4 foggedColor = lerp(textureColor, fogColor, fogFactor);

	// 出力カラーにフォグを適用
	output.color = gMaterial.color * foggedColor;

	return output;
}