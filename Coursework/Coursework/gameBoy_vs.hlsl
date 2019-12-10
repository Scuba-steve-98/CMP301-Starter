struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

OutputType main(InputType input)
{
	OutputType output;

	output.position = input.position;

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	return output;
}