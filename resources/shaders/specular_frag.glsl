#version 150

uniform sampler2D texture0;

in vec2 TexCoord;

out vec4 out_Color;

const float f = 30.0;
const float n = 0.1;

void main(void)
{
	vec3 normalAndSpec = texture(texture0, TexCoord).rgb;

	float specularBase = fract(normalAndSpec.z);
	float specularExp = normalAndSpec.z - specularBase;
	specularBase /= 0.99;

	out_Color = vec4(vec3(specularBase),1.0);
}