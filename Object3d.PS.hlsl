#include"Object3d.hlsli"

struct Material{
	float4 color;
};

ConstantBuffer<Material>gMaterial : register(b0);
Texture2D<float4> gTexture:register(t0);
SamplerState gSampler:register(s0);

struct PixelShaderOutput{
	float4 color : SV_TARGET0;
};

// フォグのパラメータを定義
cbuffer FogConstants : register(b1) {
    float4 fogColor;  // 霧の色
    float fogStart;   // 霧の開始距離
    float fogEnd;     // 霧の終了距離
};

cbuffer CameraConstants : register(b2) {
    float3 cameraPos;
};

PixelShaderOutput main(VertexShaderOutput input){
	PixelShaderOutput output;
	//テクスチャカラー
	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	output.color = gMaterial.color*textureColor;
	return output;
}