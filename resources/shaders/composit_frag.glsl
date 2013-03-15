#version 150

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

in vec2 TexCoord;

out vec3 out_Radiance;

void main(void)
{
	vec3 albedo = texture(texture0, TexCoord).rgb;
	vec3 normal = vec3(texture(texture1, TexCoord).rg, 0.0);
	normal.z = 1.0 - dot(normal.xy,normal.xy);
	vec3 light = texture(texture2, TexCoord).rgb;

	out_Radiance = albedo * light;
}