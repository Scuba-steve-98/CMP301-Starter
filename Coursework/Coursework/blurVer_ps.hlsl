
Texture2D texture0 : register(t0);
Texture2D depthTexture : register(t1);
SamplerState sampler0 : register(s0);
SamplerState depthSampler : register(s1);

cbuffer ScreenSizeBuffer : register(b0)
{
	float screenHeight;
	float3 padding;
};

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

int calculateBlurFactor(float depthValue, float focusNear, float focusFar, float blurNear, float blurFar)
{
	if (depthValue >= focusNear && depthValue <= focusFar) // Is depthValue in focus?
	{
		// No blur
		return 0;
	}
	else if (depthValue < focusNear) // Is depthValue in foreground?
	{
		float difference = focusNear - blurNear;
		float step = difference / 5.0f;

		for (int i = 0; i < 5; i++)
		{
			float min = focusNear - (step * (i + 1));
			float max = focusNear - (step * i);

			if (depthValue >= min && depthValue < max)
			{
				return (i + 1);
			}
		}

		// If not between focusNear & blurNear, then outside of range & at max blur
		return 6;
	}
	else if (depthValue > focusFar) // Is depthValue in background?
	{
		float difference = blurFar - focusFar;
		float step = difference / 5.0f;

		for (int i = 0; i < 5; i++)
		{
			float min = focusFar + (step * i);
			float max = focusFar + (step * (i + 1));

			if (depthValue >= min && depthValue < max)
			{
				return (i + 1);
			}
		}

		// If not between focusFar & blurFar, then must be outside of range & at max blur
		return 6;
	}
	else
	{
		// Error occured - don't do blur
		return 0;
	}
}

float4 main(InputType input) : SV_TARGET
{
	float4 colour;
	float depthValue;
	float weight0, weight1, weight2, weight3, weight4, weight5, weight6, weight7;

	depthValue = depthTexture.Sample(depthSampler, input.tex).r;

	int blurFactor = calculateBlurFactor(depthValue, focusNear, focusFar, blurNear, blurFar);

	if (blurFactor == 0) // No blur
	{
		colour = texture0.Sample(sampler0, input.tex);
	}
	else // Blur
	{
		switch (blurFactor)
		{
			case 1:
				weight0 = 0.682689f;
				weight1 = 0.157305f;
				weight2 = 0.00135f;
				weight3 = 0.0f;
				weight4 = 0.0f;
				weight5 = 0.0f;
				weight6 = 0.0f;
				weight7 = 0.0f;
				break;
			case 2:
				weight0 = 0.382925f;
				weight1 = 0.24173f;
				weight2 = 0.060598f;
				weight3 = 0.005977f;
				weight4 = 0.000229f;
				weight5 = 0.000003f;
				weight6 = 0.0f;
				weight7 = 0.0f;
				break;
			case 3:
				weight0 = 0.261117f;
				weight1 = 0.210786f;
				weight2 = 0.110865f;
				weight3 = 0.037975f;
				weight4 = 0.008465f;
				weight5 = 0.001227f;
				weight6 = 0.000116f;
				weight7 = 0.000007f;
				break;
			case 4:
				weight0 = 0.197448f;
				weight1 = 0.174697f;
				weight2 = 0.120999f;
				weight3 = 0.065602f;
				weight4 = 0.02784f;
				weight5 = 0.009246f;
				weight6 = 0.002403f;
				weight7 = 0.000489f;
				break;
			case 5:
				weight0 = 0.158949f;
				weight1 = 0.146884f;
				weight2 = 0.115911f;
				weight3 = 0.078109f;
				weight4 = 0.044948f;
				weight5 = 0.022087f;
				weight6 = 0.009267f;
				weight7 = 0.00332f;
				break;
			case 6:
				weight0 = 0.134032f;
				weight1 = 0.126854f;
				weight2 = 0.107545f;
				weight3 = 0.08167f;
				weight4 = 0.055555f;
				weight5 = 0.033851f;
				weight6 = 0.018476f;
				weight7 = 0.009033f;
				break;
			default:
			// Error occured - no blur
				weight0 = 1.0f;
				weight1 = 0.0f;
				weight2 = 0.0f;
				weight3 = 0.0f;
				weight4 = 0.0f;
				weight5 = 0.0f;
				weight6 = 0.0f;
				weight7 = 0.0f;
				break;
		}

		// Init colour to black
		colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Determine floating point size of a texel for a screen of this specific width
		float texelSize = 1.0f / screenHeight;

		// Add the horizontal pixels to the colour by the specific weight of each
		colour += texture0.Sample(sampler0, input.tex + float2(0.0f, texelSize * -7.0f)) * weight7;
		colour += texture0.Sample(sampler0, input.tex + float2(0.0f, texelSize * -6.0f)) * weight6;
		colour += texture0.Sample(sampler0, input.tex + float2(0.0f, texelSize * -5.0f)) * weight5;
		colour += texture0.Sample(sampler0, input.tex + float2(0.0f, texelSize * -4.0f)) * weight4;
		colour += texture0.Sample(sampler0, input.tex + float2(0.0f, texelSize * -3.0f)) * weight3;
		colour += texture0.Sample(sampler0, input.tex + float2(0.0f, texelSize * -2.0f)) * weight2;
		colour += texture0.Sample(sampler0, input.tex + float2(0.0f, texelSize * -1.0f)) * weight1;
		colour += texture0.Sample(sampler0, input.tex) * weight0;
		colour += texture0.Sample(sampler0, input.tex + float2(0.0f, texelSize * 1.0f)) * weight1;
		colour += texture0.Sample(sampler0, input.tex + float2(0.0f, texelSize * 2.0f)) * weight2;
		colour += texture0.Sample(sampler0, input.tex + float2(0.0f, texelSize * 3.0f)) * weight3;
		colour += texture0.Sample(sampler0, input.tex + float2(0.0f, texelSize * 4.0f)) * weight4;
		colour += texture0.Sample(sampler0, input.tex + float2(0.0f, texelSize * 5.0f)) * weight5;
		colour += texture0.Sample(sampler0, input.tex + float2(0.0f, texelSize * 6.0f)) * weight6;
		colour += texture0.Sample(sampler0, input.tex + float2(0.0f, texelSize * 7.0f)) * weight7;

		// Set the alpha channel to one
		colour.a = 1.0f;
	}

	//return float4(blurFactor / 6.0f, blurFactor / 6.0f, blurFactor / 6.0f, 1.0f);

	return colour;
}