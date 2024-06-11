#include"Object3d.hlsli"

ConstantBuffer< TransformationMatrix>gTransformationMat:register(b0);

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;

	output.position = mul(input.position, gTransformationMat.WVP);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3)gTransformationMat.world));
	return output;
}