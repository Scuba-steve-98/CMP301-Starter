Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

cbuffer TimeBuffer : register(b1)
{
	float time;
	float2 res;
	float padding;
}

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};


float4 main(InputType input) : SV_TARGET
{
	float blend = 1 - pow((0.5f + (0.5f * cos(time / 10))), 2.f);
float amp = cos(time / 0.5f);
float pixelSize = 8.f;

float3 choice[4];
choice[0] = (180 / 256, 210 / 256, 46 / 256);
choice[1] = (155 / 256, 195 / 256, 40 / 256);
choice[2] = (108 / 256, 151 / 256, 35 / 256);
choice[3] = (54 / 256, 112 / 256, 30 / 256);
float3 Line = choice[0] * 1.6f;

float2 co_ord = input.tex / res.xy;
float2 resolution = float2(res.x / 8.f, res.y / 8.f);

float2 uv = lerp(co_ord, (floor(co_ord * resolution) / resolution), blend);

float2 pix = uv * resolution;

float3 orig = texture0.Sample(Sampler0, uv).rgb;
float3 col = Line;
float2 line_width = float2(1.0 / res.x, 1.0 / res.y);

float2 pix_thresh = lerp((0.f), line_width, blend);

bool is_pixel = (co_ord.x - uv.x) > pix_thresh.x && (co_ord.y - uv.y) > pix_thresh.y;

// good old times when some pixel columns were broken. remember? :D
bool is_not_broken = int(pix.x) != int(resolution.x) - 2 && (int(pix.x) != int(resolution.x) - 4 || frac(4.0 * sin(time / 10.)) > 0.95) && int(pix.x) != 1;

if (is_pixel && is_not_broken)
{
	float val = (orig.r + orig.g + orig.b) / 3.0;
	int shade = min(3, int(4.0 * log(1.0 + val * 1.71))); // log(1+x*(e-1))
	col = choice[shade];

	// the following is optional: it's just some gradient across the screen.
	// like a bit of sunlight screwing up the contrast :D
	float2 offs = 3. * float2(0.5 + 0.5 * sin(time), 0.5 + 0.5 * cos(time / 1.2));
	float2 cen = float2(0.5, 0.5) + offs;
	//col += float3(0.1, 0.14, 0.01) * (0.5 + 0.5 * sin(pow(tan(cen.x, cen.y), -1))); // slight loss of contrast, moving with time
	col -= float3(0.8, 1.0, 0.5) * 5.0 * sin(amp) * length(uv - co_ord); // slight pixel 3d-ness
}

// slight vignette
col *= 1.1 - 0.3 * distance(co_ord, float2(0.5, 0.5));

col = lerp(orig, col, blend);

float4 colour = float4(col, 1.0);
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	return colour;
}