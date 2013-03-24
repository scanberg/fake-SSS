#version 150

uniform sampler2D texture0;
uniform sampler2D texture1;

in vec2 TexCoord;

out vec3 out_Tonemapped;

const float W = 11.2;
 
vec3 Uncharted2Tonemap(vec3 x)
{
	const float A = 0.15;
	const float B = 0.50;
	const float C = 0.10;
	const float D = 0.20;
	const float E = 0.02;
	const float F = 0.30;

	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 gaussianSampler(sampler2D tex, vec2 coord, float lod)
{
	vec3 val = vec3(0.0);

	const vec2 texScale = 1.0 / vec2(1024.0, 768.0);

	val += textureLod(tex,coord + vec2(-1,-1)*texScale,	lod).rgb;
	val += textureLod(tex,coord + vec2( 0,-1)*texScale,	lod).rgb * 2.0;
	val += textureLod(tex,coord + vec2( 1,-1)*texScale,	lod).rgb;

	val += textureLod(tex,coord + vec2(-1, 0)*texScale,	lod).rgb * 2.0;
	val += textureLod(tex,coord + vec2( 0, 0)*texScale,	lod).rgb * 4.0;
	val += textureLod(tex,coord + vec2( 1, 0)*texScale,	lod).rgb * 2.0;

	val += textureLod(tex,coord + vec2(-1, 1)*texScale,	lod).rgb;
	val += textureLod(tex,coord + vec2( 0, 1)*texScale,	lod).rgb * 2.0;
	val += textureLod(tex,coord + vec2( 1, 1)*texScale,	lod).rgb;

	return val / 16.0;
}

void main(void)
{
	vec3 texColor = texture(texture0, TexCoord).rgb;
	vec3 bloomColor = vec3(0.0f);

	bloomColor += textureLod(texture1, TexCoord, 0).rgb;
	bloomColor += textureLod(texture1, TexCoord, 1).rgb;
	bloomColor += textureLod(texture1, TexCoord, 2).rgb;
	bloomColor += textureLod(texture1, TexCoord, 3).rgb;
	bloomColor += textureLod(texture1, TexCoord, 4).rgb;

	// Hardcoded Exposure Adjustment
	//texColor *= 2.0;
	float ExposureBias = 2.0f;

	vec3 whiteScale = 1.0f/Uncharted2Tonemap(vec3(W));

	const float bloom = 0.5;

	vec3 final = texColor + bloom * bloomColor;

	vec3 curr = Uncharted2Tonemap(ExposureBias*final);
	
	vec3 color = curr*whiteScale;

	out_Tonemapped = color;
	//out_Tonemapped = texColor;
}