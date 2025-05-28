Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct VertexShaderOutput {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

float4 main(VertexShaderOutput input) : SV_TARGET {
	const int kNumSamples = 10;

	float2 centerPos = float2(0.5f, 0.5f);
	float width = 0.08f;

	float2 dir = input.texcoord - centerPos;
	float2 step = dir * width / kNumSamples;

	float3 accumColor = float3(0.0f, 0.0f, 0.0f);
	float totalWeight = 0.0f;

	for(int i = 0; i < kNumSamples; ++i) {
		float2 sampleUV = input.texcoord - step * i;
		float weight = 1.0f - (float)i / kNumSamples;
		accumColor += gTexture.Sample(gSampler, sampleUV).rgb * weight;
		totalWeight += weight;
	}

	accumColor /= totalWeight;

	return float4(accumColor, 1.0f);
}