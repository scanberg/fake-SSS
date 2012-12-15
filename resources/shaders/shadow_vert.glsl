#version 150

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 in_position;
in vec3 in_normal;

out vec3 Normal;
out vec3 ViewDir;
out vec3 WorldPos;
out vec4 ShadowProj;

void main(void)
{
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(in_position, 1.0);

	Normal = mat3(viewMatrix * modelMatrix) * in_normal;
	ViewDir = ( viewMatrix * modelMatrix * vec4(in_position, 1.0) ).xyz;
	WorldPos = (modelMatrix * vec4(in_position, 1.0)).xyz;

	ShadowProj = textureMatrix * vec4(in_position + in_normal*0.05, 1.0);
}