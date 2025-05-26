#include"Particle.hlsli"

///////////////////////////////////////////////////////////////////////////////
//                            structs
///////////////////////////////////////////////////////////////////////////////
struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

struct ParticleForGPU {
	float4x4 world;
	float4 color;
};

///////////////////////////////////////////////////////////////////////////////
//                            tables
///////////////////////////////////////////////////////////////////////////////
StructuredBuffer<ParticleForGPU> gParticle : register(t0);

///////////////////////////////////////////////////////////////////////////////
//                              main
///////////////////////////////////////////////////////////////////////////////
VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID) {
	VertexShaderOutput output;

	float4 worldPos = mul(input.position, gParticle[instanceId].world);
	
	output.position = mul(worldPos, gCamera.viewProjection);
	output.texcoord = input.texcoord;
	output.color = gParticle[instanceId].color;
	return output;
}