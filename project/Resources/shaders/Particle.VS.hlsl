#include"Particle.hlsli"

///////////////////////////////////////////////////////////////////////////////
//                            structs
///////////////////////////////////////////////////////////////////////////////
struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

struct ParticleConstants {
	float3 position;
	float size;
	float4 color;
	int meshID;
};

struct Camera {
	float4x4 view;
	float4x4 projection;
	float4x4 viewProjection;
	float3 cameraPosition;
	float3 camRight; // ViewMatrixのX列
	float3 camUp; // ViewMatrixのY列
};

///////////////////////////////////////////////////////////////////////////////
//                            cbuffers
///////////////////////////////////////////////////////////////////////////////
ConstantBuffer<Camera> gCamera : register(b0);


///////////////////////////////////////////////////////////////////////////////
//                            tables
///////////////////////////////////////////////////////////////////////////////
StructuredBuffer<ParticleConstants> gParticle : register(t0);


float2 quad[4] = {
	float2(-0.5f, 0.5f),
	float2(0.5f, 0.5f),
	float2(-0.5f, -0.5f),
	float2(0.5f, -0.5f)
};

///////////////////////////////////////////////////////////////////////////////
//                              main
///////////////////////////////////////////////////////////////////////////////
VertexShaderOutput main(uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID) {
	ParticleConstants p = gParticle[instanceID];

	float2 offset = quad[vertexID] * p.size;
	float3 worldPos = p.position + gCamera.camRight * offset.x + gCamera.camUp * offset.y;

	VertexShaderOutput output;
	output.position = mul(float4(worldPos, 1.0f), gCamera.viewProjection);
	output.texcoord = float2((vertexID % 2), (vertexID / 2));
	output.color = p.color;
	return output;
}