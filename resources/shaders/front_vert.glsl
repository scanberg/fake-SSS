#version 150

uniform mat4 modelMatrix;
uniform mat4 invModelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec3 camPos;

in vec3 in_position;
in vec3 in_normal;
in vec3 in_tangent;
in vec2 in_texCoord;

out vec3 Position;
out vec3 Normal;
out vec3 Tangent;
out vec3 WorldPos;
out vec2 TexCoord;
out vec3 ViewSpaceCoord;
out vec3 CameraInObjectSpace;

void main(void)
{
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(in_position, 1.0);

	mat3 normalMat = mat3(viewMatrix * modelMatrix);

	Position = in_position;
	Normal = normalMat * in_normal;
	Tangent = normalMat * in_tangent;

	WorldPos = (modelMatrix * vec4(in_position, 1.0)).xyz;

	ViewSpaceCoord = (viewMatrix * modelMatrix * vec4(in_position, 1.0)).xyz;

	CameraInObjectSpace = (invModelMatrix * vec4(camPos,1.0)).xyz;

	TexCoord = in_texCoord;
}