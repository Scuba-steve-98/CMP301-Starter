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

OutputType main(InputType input)
{
    OutputType output;
#define AMPLITUDE 20
#define DISPLACE 0.14f

    //float2 left, right, top, bottom, centre;

    float4 height, heightNormalA, heightNormalB;
    //float2 texB, texC;

    height = texture0.SampleLevel(sampler0, input.tex, 0) - DISPLACE;
    input.position.y = (height.x * AMPLITUDE);
	
    //texB.x = input.tex.x + 0.01;
    //texB.y = input.tex.y;
    //heightNormalA = texture0.SampleLevel(sampler0, texB, 0);

    //texC.x = input.tex.x;
    //texC.y = input.tex.y + 0.01;
    //heightNormalB = texture0.SampleLevel(sampler0, texC, 0);

    //input.normal.x = input.position.y - (heightNormalA.x * AMPLITUDE);
    //input.normal.z = input.position.y - (heightNormalB.x * AMPLITUDE);
    //input.normal.y = normalize(cross(input.normal.x, input.normal.z));

	#define gTexelCellSpaceU (1 / 2048)
	#define gTexelCellSpaceV (1 / 2048)
	//#define worldSpacing 0.1f

    float worldSpacing = distance(texture0.SampleLevel(sampler0, input.tex, 0), texture0.SampleLevel(sampler0, input.tex + 0.1, 0));

    float2 leftTex = input.tex + float2(-gTexelCellSpaceU, 0.0f);
    float2 rightTex = input.tex + float2(gTexelCellSpaceU, 0.0f);
    float2 bottomTex = input.tex + float2(0.0f, gTexelCellSpaceV);
    float2 topTex = input.tex + float2(0.0f, -gTexelCellSpaceV);

    float leftY = texture0.SampleLevel(sampler0, leftTex, 0).r;
    float rightY = texture0.SampleLevel(sampler0, rightTex, 0).r;
    float bottomY = texture0.SampleLevel(sampler0, bottomTex, 0).r;
    float topY = texture0.SampleLevel(sampler0, topTex, 0).r;

    float3 tangent = normalize(float3(2.0f * worldSpacing, rightY - leftY, 0.0f));
    float3 bitan = normalize(float3(0.0f, bottomY - topY, -2.0f * worldSpacing));
    float3 normalW = cross(tangent, bitan);

    input.normal = normalW;

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