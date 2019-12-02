// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
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
};


float height()
{
    float val = 1;
    return val;
}

OutputType main(InputType input)
{
    OutputType output;
#define AMPLITUDE 20
//#define DISPLACE 0.14f
	#define OFFSET 1.0f
	#define UV_OFFSET (1/185)
	
    float4 height, heightNormalA, heightNormalB;
    float2 north, south, east, west;
    float2 texB, texC;

 //   north.x = input.tex.x;
	//north.y = input.tex.y + UV_OFFSET;

	//east.x = input.tex.x + UV_OFFSET;
 //   east.y = input.tex.y;

 //   south.x = input.tex.x;
	//south.y = input.tex.y - UV_OFFSET;

	//west.x = input.tex.x - UV_OFFSET;
 //   west.y = input.tex.y;

    height = texture0.SampleLevel(sampler0, input.tex, 0);
    input.position.y = (height.x * AMPLITUDE);

 //   float3 left, right, top, bottom, centre, tc, rc, bc, lc, input_normal;
 //   top = (input.position.x, (texture0.SampleLevel(sampler0, north, 0) * AMPLITUDE), (input.position.z + OFFSET));
 //   right = ((input.position.x + OFFSET), (texture0.SampleLevel(sampler0, east, 0) * AMPLITUDE), input.position.z);
 //   bottom = (input.position.x, (texture0.SampleLevel(sampler0, south, 0) * AMPLITUDE), (input.position.z - OFFSET));
 //   left = ((input.position.x - OFFSET), (texture0.SampleLevel(sampler0, west, 0) * AMPLITUDE), input.position.z);
 //   centre = (input.position.x, input.position.y, input.position.z);

	
	//// vectors
	//// vectors
 //   tc = (top - centre);
 //   rc = (right - centre);
 //   bc = (bottom - centre);
 //   lc = (left - centre);
	
	//top = normalize(cross(tc, rc));
	//right = normalize(cross(rc, bc));
	//bottom = normalize(cross(bc, lc));
	//left = normalize(cross(lc, tc));

 //   input_normal = (top + right + bottom + left) / 4;

 //   input.normal = input_normal;

	texB.x = input.tex.x + 0.01;
	texB.y = input.tex.y;
	heightNormalA = texture0.SampleLevel(sampler0, texB, 0);

	texC.x = input.tex.x;
	texC.y = input.tex.y + 0.01;
	heightNormalB = texture0.SampleLevel(sampler0, texC, 0);

	input.normal.x = input.position.y - (heightNormalA.x * AMPLITUDE);
	input.normal.z = input.position.y - (heightNormalB.x * AMPLITUDE);
	input.normal.y = 1;

	//#define gTexelCellSpaceU (1 / 185)
	//#define gTexelCellSpaceV (1 / 185)
	//#define worldSpacing 1

	////float worldSpacing = distance(texture0.SampleLevel(sampler0, input.tex, 0), texture0.SampleLevel(sampler0, input.tex + 0.1, 0));

	//float2 leftTex = input.tex + float2(-gTexelCellSpaceU, 0.0f);
	//float2 rightTex = input.tex + float2(gTexelCellSpaceU, 0.0f);
	//float2 bottomTex = input.tex + float2(0.0f, gTexelCellSpaceV);
	//float2 topTex = input.tex + float2(0.0f, -gTexelCellSpaceV);

	//float leftY = texture0.SampleLevel(sampler0, leftTex, 0).r;
	//float rightY = texture0.SampleLevel(sampler0, rightTex, 0).r;
	//float bottomY = texture0.SampleLevel(sampler0, bottomTex, 0).r;
	//float topY = texture0.SampleLevel(sampler0, topTex, 0).r;

	//float3 tangent = normalize(float3(2.0f * worldSpacing, rightY - leftY, 0.0f));
	//float3 bitan = normalize(float3(0.0f, bottomY - topY, -2.0f * worldSpacing));
	//float3 normalW = cross(tangent, bitan);

	//input.normal = normalW;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    return output;
}