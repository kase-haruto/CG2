#include"NonTextureObject.hlsli"

ConstantBuffer< TransformationMatrix>gTransformationMat:register(b0);

VertexShaderOutput main(VertexShaderInput input){
	VertexShaderOutput output;

	output.position = mul(input.position, gTransformationMat.WVP);
	output.normal = normalize(mul(input.normal, (float3x3)gTransformationMat.world));
	return output;
}