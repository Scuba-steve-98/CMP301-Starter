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
	#define OFFSET .1f
    OutputType output;
	// find co-ordinates
    float3 north, east, south, west, centre;
	// manipulate vertex co-ords
    input.position.y = calc(input.position.x, input.position.z);

    north.x = input.position.x;
    east.x = input.position.x + OFFSET;
    south.x = input.position.x;
    west.x = input.position.x - OFFSET;
    centre.x = input.position.x;

    north.y = calc(input.position.x, input.position.y + OFFSET);
    east.y = calc(input.position.x + OFFSET, input.position.y);
    south.y = calc(input.position.x, input.position.y - OFFSET);
    west.y = calc(input.position.x - OFFSET, input.position.y);
    centre.y = input.position.y;

    north.z = input.position.z + OFFSET;
    east.z = input.position.z;
    south.z = input.position.z - OFFSET;
    west.z = input.position.z;
    centre.z = input.position.z;

    //input.position.y = amplitude * sin(freq * ((pow(input.position.x, 2) / 2.34) + pow((exp(-input.position.x)), 2) + pow(input.position.z, 8 / 5) + (time * speed)));

    //north = (input.position.x, (amplitude * sin(freq * ((pow(input.position.x, 2) / 2.34) + pow((exp(-input.position.x)), 2) + pow((input.position.z + 0.1f), 8 / 5) + (time * speed)))), (input.position.z + 0.1f));
    //east = ((input.position.x + 0.1f), (amplitude * sin(freq * ((pow(input.position.x + 0.1f, 2) / 2.34) + pow((exp(-(input.position.x + 0.1f))), 2) + pow((input.position.z), 8 / 5) + (time * speed)))), input.position.z);
    //south = (input.position.x, (amplitude * sin(freq * ((pow(input.position.x, 2) / 2.34) + pow((exp(-input.position.x)), 2) + pow((input.position.z - 0.1f), 8 / 5) + (time * speed)))), (input.position.z - 0.1f));
    //west = ((input.position.x - 0.1f), (amplitude * sin(freq * ((pow(input.position.x - 0.1f, 2) / 2.34) + pow((exp(-(input.position.x - 0.1f))), 2) + pow((input.position.z), 8 / 5) + (time * speed)))), input.position.z);
    //centre = (input.position.x, input.position.y, input.position.z);

   // calculate vectors
    float3 north_centre, east_centre, south_centre, west_centre;

    north_centre.x = north.x - centre.x;
    east_centre.x = east.x - centre.x;
    south_centre.x = south.x - centre.x;
    west_centre.x = west.x - centre.x;

    north_centre.y = north.y - centre.y;
    east_centre.y = east.y - centre.y;
    south_centre.y = south.y - centre.y;
    west_centre.y = west.y - centre.y;

    north_centre.z = north.z - centre.z;
    east_centre.z = east.z - centre.z;
    south_centre.z = south.z - centre.z;
    west_centre.z = west.z - centre.z;

    //north_centre = centre - north;
    //east_centre = centre - east;
    //south_centre = centre - south;
    //west_centre = centre - west;
	   
	// calculate cross product
    float3 crossNE, crossES, crossSW, crossWN;

    crossNE = /*normalize*/(cross(north_centre, east_centre)); //0
    //crossNE = normalize(cross(east_centre, north_centre));
	   
    crossES = /*normalize*/(cross(east_centre, south_centre)); //0		
    //crossES = normalize(cross(south_centre, east_centre));
    
    crossSW = /*normalize*/(cross(south_centre, west_centre)); //0	
    //crossSW = normalize(cross(west_centre, south_centre));
	
    crossWN = /*normalize*/(cross(west_centre, north_centre)); //0
    //crossWN = normalize(cross(north_centre, west_centre));

	// normalize
    float3 normal1, normal2, normal3, normal4, normal_avg;
    //normal1 = normalize(crossNE);
    //normal2 = normalize(crossES);
    //normal3 = normalize(crossSW);
    //normal4 = normalize(crossWN);
    normal_avg = normalize(crossNE + crossES + crossSW + crossWN / 4);

	// calculate average
    float3 average_normal;
    //average_normal = (normal1 + normal2 + normal3 + normal4) / 4;
    //average_normal = (crossNE + crossES + crossSW + crossWN) / 4;

	//set normals
    input.normal = normal_avg;

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