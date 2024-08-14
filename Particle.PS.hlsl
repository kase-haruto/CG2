#include"Particle.hlsli"

Texture2D<float4> gTexture:register(t0);
SamplerState gSampler:register(s0);
ConstantBuffer<Material>gMaterial : register(b0);

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;

	// UV座標を変換
	float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);

	// テクスチャサンプルを取得
	float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	output.color = gMaterial.color * textureColor*input.color;
	//putput.colorの値が0の時にpixelを破棄
	if (output.color.a == 0.0){
		discard;
	}
	return output;
}