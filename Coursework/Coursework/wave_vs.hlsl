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

cbuffer TimeBuffer : register(b1)
{
    float time;
    float amplitude;
    float speed;
    float freq;
}

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

float calc(float x, float z)
{
    float val = amplitude * sin(freq * ((pow(x, 2) / 2.34) + pow((exp(-x)), 2) + pow(z, 8 / 5) + (time * speed)));

    return val;
}

OutputType main(InputType input)
{
	#define OFFSET 1.0f
    OutputType output;
	// find co-ordinates
    float3 north, east, south, west, centre;
	// manipulate vertex co-ords
    input.position.y = calc(input.position.x, input.position.z);

    //north.x = input.position.x;
    //east.x = input.position.x + OFFSET;
    //south.x = input.position.x;
    //west.x = input.position.x - OFFSET;
    //centre.x = input.position.x;

    //north.y =	calc(input.position.x,				(input.position.z + OFFSET));
    //east.y =	calc((input.position.x + OFFSET),	input.position.z);
    //south.y =	calc(input.position.x,				(input.position.z - OFFSET));
    //west.y =	calc((input.position.x - OFFSET),	input.position.z);
    //centre.y =	calc(input.position.x,				input.position.z);

    //north.z = input.position.z + OFFSET;
    //east.z = input.position.z;
    //south.z = input.position.z - OFFSET;
    //west.z = input.position.z;
    //centre.z = input.position.z;
	
	north = float4(input.position.x, calc(input.position.x, (input.position.z + OFFSET)), (input.position.z + OFFSET), input.position.w);
	east = float4((input.position.x + OFFSET), calc((input.position.x + OFFSET), input.position.z), input.position.z, input.position.w);
	south = float4(input.position.x, calc(input.position.x, (input.position.z - OFFSET)), (input.position.z - OFFSET), input.position.w);
	west = float4((input.position.x - OFFSET), calc((input.position.x - OFFSET), input.position.z), input.position.z, input.position.w);
	
   // calculate vectors
    float3 north_centre, east_centre, south_centre, west_centre;

	north_centre = normalize(north - input.position);
	east_centre = normalize(east - input.position);
	south_centre = normalize(south - input.position);
	west_centre = normalize(west - input.position);
	   
	// calculate cross product
    float3 crossNE, crossES, crossSW, crossWN;

	crossNE = cross(north_centre, east_centre);
	   
	crossES = cross(east_centre, south_centre);
    
	crossSW = cross(south_centre, west_centre);
	
	crossWN = cross(west_centre, north_centre);

	// normalize
	float3 normal1, normal2, normal3, normal4;
	normal1 = normalize(crossNE);
	normal2 = normalize(crossES);
	normal3 = normalize(crossSW);
	normal4 = normalize(crossWN);

	// calculate average
    float3 average_normal;
    //average_normal = (crossNE + crossES + crossSW + crossWN) / 4;
	average_normal = (normal1 + normal2 + normal3 + normal4) / 4;

	//set normals
	//input.normal.x = average_normal.x;
	//input.normal.y = average_normal.y;
	input.normal = average_normal;

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