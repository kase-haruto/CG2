struct VertexShaderOutput {
	float32_t4 position : SV_POSITION;
};

struct VertexShaderInput {
	float32_t4 position : POSITION0
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutout output;
	output.position = input.position;
	return output;
}