#include"Object3d.hlsli"

VertexShaderOutput main(VertexShaderInput input) {
	//ƒ[ƒ‹ƒhÀ•WŒn‚Ìpos‚ğæ“¾
	float4 worldPos = mul(input.position, world);
	
	VertexShaderOutput output;

	output.position = mul(input.position, mul(world,mul(view,projection)));
	output.worldPosition = worldPos;
	output.texcoord = input.texcoord;

	return output;
}