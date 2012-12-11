#version 150
 
precision highp float; // needed only for version 1.30

uniform sampler2D texture0;

in vec2 TexCoord;

out vec4 out_Color;

const float f = 50.0;
const float n = 0.1;

float linearizeDepth(float depth)
{
	return (2.0 * n) / (f + n - depth * (f - n));
}

void main(void)
{
	//vec4 textureColor = vec4(texture(texture0, TexCoord).r);
	vec4 textureColor = vec4( linearizeDepth( texture(texture0, TexCoord).r ) );

	out_Color = textureColor;
}