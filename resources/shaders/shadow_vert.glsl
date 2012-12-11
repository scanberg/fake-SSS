#version 150

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 in_position;
in vec3 in_normal;

out vec3 Normal;
out vec3 ViewDir;
out vec4 ShadowProj;

void main(void)
{
	gl_Position = projMatrix * viewMatrix * vec4(in_position, 1.0);

	Normal = mat3(viewMatrix) * in_normal;
	ViewDir = ( viewMatrix * vec4(in_position,1.0) ).xyz;
	ShadowProj = textureMatrix * vec4(in_position, 1);
}