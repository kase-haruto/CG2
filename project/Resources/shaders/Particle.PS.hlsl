#include "Particle.hlsli"

///////////////////////////////////////////////////////////////////////////////
//                            structs
///////////////////////////////////////////////////////////////////////////////
//マテリアル
struct Material {
	float4 color;
	float4x4 uvTransform;
};

///////////////////////////////////////////////////////////////////////////////
//                            出力
///////////////////////////////////////////////////////////////////////////////
struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

///////////////////////////////////////////////////////////////////////////////
//                            cbuffers
///////////////////////////////////////////////////////////////////////////////
ConstantBuffer<Material> gMaterial : register(b1);

///////////////////////////////////////////////////////////////////////////////
//                            tables
///////////////////////////////////////////////////////////////////////////////
Texture2D<float4> gTexture : register(t1);

///////////////////////////////////////////////////////////////////////////////
//                            samplers
///////////////////////////////////////////////////////////////////////////////
SamplerState gSampler : register(s0);

///////////////////////////////////////////////////////////////////////////////
//                            main
///////////////////////////////////////////////////////////////////////////////
PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

	// UV座標を変換
	float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	// テクスチャサンプル
	float4 texColor = gTexture.Sample(gSampler, transformedUV.xy);
	// 合成
	float4 baseColor = gMaterial.color * texColor * input.color;
	// トーンマッピング
	float exposure = 1.0f;
	float3 toneMapped = baseColor.rgb * exposure / (baseColor.rgb * exposure + 1.0f);
	// ガンマ補正
	float3 gammaCorrected = pow(toneMapped, 1.0 / 2.2);

	output.color = float4(gammaCorrected, baseColor.a);

	// 完全透明なら破棄
	if(output.color.a <= 0.01f) {
		discard;
	}

	return output;
}
