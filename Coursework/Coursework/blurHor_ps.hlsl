Texture2D sceneTexture : register(t0);
Texture2D depthTexture : register(t1);
SamplerState texSampler : register(s0);
SamplerState depthSampler : register(s1);

cbuffer ScreenSizeBuffer : register(b0)
{
	float screenWidth;
	float3 padding;
}

cbuffer DepthBlurBuffer : register(b1)
{
	float focusNear;
	float focusFar;
	float blurNear;
	float blurFar;
}

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};


float4 main(InputType input) : SV_TARGET
{
	float depth = depthTexture.Sample(depthSampler, input.tex).r;
	int blurFactor = 0;
	
	if (focusNear <= depth <= focusFar)
	{
		blurFactor = 0;
	}
	else if (depth < focusNear)
	{
		float step = (focusNear - blurNear) / 4.0f;
		
		blurFactor = 5;
		
		for (int i = 0; i < 4; i++)
		{
			float min = focusNear - (step * (i + 1));
			float max = focusNear - (step * i);

			if (min < depth < max)
			{
				blurFactor = (i + 1);
			}
		}
	}
	else if (depth > focusFar)
	{
		float step = (blurFar - focusFar) / 4.0f;
		
		blurFactor = 5;
		
		for (int i = 0; i < 4; i++)
		{
			float min = focusFar + (step * i);
			float max = focusFar + (step * (i + 1));

			if (min < depth < max)
			{
				blurFactor = (i + 1);
			}
		}
	}
	
	float4 colour;
	float weight[7];
	switch (blurFactor)
	{
		case 0:
			colour = sceneTexture.Sample(texSampler, input.tex);
			break;
		case 1:
			weight[0] = 0.62518631f;
			weight[1] = 0.174615f;
			weight[2] = 0.0016546f;
			weight[3] = 0.0000161f;
			weight[4] = 0.0f;
			weight[5] = 0.0f;
			weight[6] = 0.0f;
			break;
		case 2:
			weight[0] = 0.3746844f;
			weight[1] = 0.2715641f;
			weight[2] = 0.05481f;
			weight[3] = 0.006f;
			weight[4] = 0.0003f;
			weight[5] = 0.00008f;
			weight[6] = 0.0f;
			break;
		case 3:
			weight[0] = 0.248614f;
			weight[1] = 0.216612f;
			weight[2] = 0.124647f;
			weight[3] = 0.0548f;
			weight[4] = 0.00645f;
			weight[5] = 0.001776f;
			weight[6] = 0.000084f;
			break;
		case 4:
			weight[0] = 0.200154f;
			weight[1] = 0.165483f;
			weight[2] = 0.119f;
			weight[3] = 0.07f;
			weight[4] = 0.024071f;
			weight[5] = 0.009067f;
			weight[6] = 0.002201f;
			break;
		case 5:
			weight[0] = 0.16f;
			weight[1] = 0.139475f;
			weight[2] = 0.12716f;
			weight[3] = 0.080379f;
			weight[4] = 0.045f;
			weight[5] = 0.020173f;
			weight[6] = 0.010098f;
			break;
		default:
			weight[0] = 1.0f;
			weight[1] = 0.0f;
			weight[2] = 0.0f;
			weight[3] = 0.0f;
			weight[4] = 0.0f;
			weight[5] = 0.0f;
			weight[6] = 0.0f;
			break;
	}

	// Init colour to black
	colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Determine floating point size of a texel for a screen of this specific width
	float texelSize = 1.0f / screenWidth;
		
	for (int i = -6; i < 7; i++)
	{
		int x = i;
		if (x < 0)
			x = -i;
		
		colour += sceneTexture.Sample(texSampler, input.tex + float2(texelSize * i, 0.0f)) * weight[x];
	}

	// Set the alpha channel to one
	colour.a = 1.0f;
	
	return colour;
}