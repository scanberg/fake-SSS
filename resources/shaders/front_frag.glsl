#version 150

uniform sampler2D texture0; 		// Diffuse color map
uniform sampler2D texture1;			// Normal map

in vec3 Normal;
in vec3 Tangent;
in vec3 WorldPos;
in vec2 TexCoord;
in vec3 ViewSpaceCoord;

out vec4 out_Color[4];

void main(void)
{
	// Transform into [-1, 1]
	vec3 textureNormal = texture(texture1,TexCoord).rgb * 2.0 - 1.0;

	vec3 n = normalize(Normal);
	vec3 t = normalize(Tangent);
	vec3 b = cross(n,t);

	vec3 viewSpaceNormal = normalize(textureNormal.x * t + textureNormal.y * b + textureNormal.z * n);

	out_Color[0] = vec4(texture(texture0,TexCoord).rgb/2.2,0.0);
	out_Color[1] = vec4(viewSpaceNormal.xy, TexCoord);
	out_Color[2] = vec4(WorldPos,0.0);
}