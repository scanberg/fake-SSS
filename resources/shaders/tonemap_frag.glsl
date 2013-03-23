#version 150

uniform sampler2D texture0;

in vec2 TexCoord;

out vec3 out_Tonemapped;

const float W = 111.2;
 
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

void main(void)
{
	vec3 texColor;
	texColor = texture(texture0, TexCoord).rgb;
	//texColor += textureLod(texture0, TexCoord, 1.5).rgb;
	//texColor += textureLod(texture0, TexCoord, 2.5).rgb;
	//texColor += textureLod(texture0, TexCoord, 3.5).rgb;

	// Hardcoded Exposure Adjustment
	// texColor *= 16.0;
	float ExposureBias = 2.0f;

	vec3 whiteScale = 1.0f/Uncharted2Tonemap(vec3(W));

	vec3 curr = Uncharted2Tonemap(ExposureBias*texColor);
	
	vec3 color = curr*whiteScale;

	out_Tonemapped = color;
	//out_Tonemapped = texColor;
}