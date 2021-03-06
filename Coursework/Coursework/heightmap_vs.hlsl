// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};



struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos : TEXCOORD1;
};

//float height()
//{
//	float val = 1;
//	return val;
//}

OutputType main(InputType input)
{
    OutputType output;
#define AMPLITUDE 20
	#define OFFSET 1.0f
	#define UV_OFFSET (1/185)
	
    float4 height, heightNormalA, heightNormalB;
    float2 north, south, east, west;
    float2 texB, texC;


    height = texture0.SampleLevel(sampler0, input.tex, 0);
    input.position.y = (height.x * AMPLITUDE);


	texB.x = input.tex.x + 0.01;
	texB.y = input.tex.y;
	heightNormalA = texture0.SampleLevel(sampler0, texB, 0);

	texC.x = input.tex.x;
	texC.y = input.tex.y + 0.01;
	heightNormalB = texture0.SampleLevel(sampler0, texC, 0);

	input.normal.x = input.position.y - (heightNormalA.x * AMPLITUDE);
	input.normal.z = input.position.y - (heightNormalB.x * AMPLITUDE);
	input.normal.y = 1;


	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
	
	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPos = mul(input.position, worldMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);

    return output;
}