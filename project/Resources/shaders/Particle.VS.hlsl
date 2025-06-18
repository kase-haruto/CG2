#include"Particle.hlsli"

///////////////////////////////////////////////////////////////////////////////
//                            structs
///////////////////////////////////////////////////////////////////////////////
struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

struct ParticleData {
	float3 position;
	float size;
	float4 color;
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
StructuredBuffer<ParticleData> gParticle : register(t0);



///////////////////////////////////////////////////////////////////////////////
//                            main
///////////////////////////////////////////////////////////////////////////////
VertexShaderOutput main(uint vertexID : SV_VertexID,
                        uint instanceID : SV_InstanceID)
{
    ParticleData p = gParticle[instanceID];

    // 0,1,2,3 → (-0.5,0.5),(0.5,0.5),(-0.5,-0.5),(0.5,-0.5)
    float2 corner = float2((vertexID & 1) ? 0.5f : -0.5f,
                            (vertexID & 2) ? -0.5f : 0.5f);
    float2 offset = corner * p.size;
    float3 worldPos = p.position
                    + gCamera.camRight * offset.x
                    + gCamera.camUp * offset.y;

    VertexShaderOutput o;
    o.position = mul(float4(worldPos, 1), gCamera.viewProjection);
    o.texcoord = float2(vertexID & 1, vertexID >> 1);
    o.color = p.color;
    return o;
}