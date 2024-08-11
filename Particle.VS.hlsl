#include"Particle.hlsli"

StructuredBuffer<TransformationMatrix> gTransformationMatrices : register(t0);

VertexShaderOutput main(VertexShaderInput input,uint32_t instanceId : SV_InstanceID){
	VertexShaderOutput output;

	output.position = mul(input.position, gTransformationMatrices[instanceId].WVP);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3)gTransformationMatrices[instanceId].world));
	return output;
}