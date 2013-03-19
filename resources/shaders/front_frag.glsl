#version 150

uniform sampler2D texture0; 		// Diffuse color map
uniform sampler2D texture1;			// Normal map

in vec3 Normal;
in vec3 Tangent;
in vec3 WorldPos;
in vec2 TexCoord;

out vec3 out_Color[3];

void main(void)
{
	// Transform into [-1, 1]
	vec3 textureNormal = texture(texture1,TexCoord).rgb * 2.0 - 1.0;

	vec3 n = normalize(Normal);
	vec3 t = normalize(Tangent);
	vec3 b = cross(n,t);

	vec3 viewSpaceNormal = textureNormal.x * t + textureNormal.y * b + textureNormal.z * n;

	out_Color[0] = texture(texture0,TexCoord).rgb;
	out_Color[1] = viewSpaceNormal;
	out_Color[2] = WorldPos;
}