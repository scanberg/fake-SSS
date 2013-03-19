#version 150

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 in_position;
in vec3 in_normal;
in vec3 in_tangent;
in vec2 in_texCoord;

out vec3 Normal;
out vec3 Tangent;
out vec3 WorldPos;
out vec2 TexCoord;

void main(void)
{
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(in_position, 1.0);

	mat3 normalMat = mat3(viewMatrix * modelMatrix);

	Normal = normalMat * in_normal;
	Tangent = normalMat * in_tangent;

	WorldPos = (modelMatrix * vec4(in_position, 1.0)).xyz;

	TexCoord = in_texCoord;
}