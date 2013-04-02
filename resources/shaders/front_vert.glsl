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
out vec3 WorldNormal;
out vec2 TexCoord;
out vec3 ViewSpaceCoord;
out vec3 CameraInObjectSpace;
out vec3 EyeInTangentSpace;

void main(void)
{
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(in_position, 1.0);

	mat3 normalMat = mat3(viewMatrix * modelMatrix);

	Position = in_position;
	Normal = normalize(normalMat * in_normal);
	Tangent = normalize(normalMat * in_tangent);
	vec3 bitangent = cross(Normal, Tangent);

	WorldPos = (modelMatrix * vec4(in_position, 1.0)).xyz;
	WorldNormal = (modelMatrix * vec4(in_normal, 0.0)).xyz;

	ViewSpaceCoord = (viewMatrix * modelMatrix * vec4(in_position, 1.0)).xyz;

	CameraInObjectSpace = (invModelMatrix * vec4(camPos,1.0)).xyz;

	vec3 eyevec = -ViewSpaceCoord;
	EyeInTangentSpace.x = dot(eyevec, Tangent);
	EyeInTangentSpace.y = dot(eyevec, bitangent);
	EyeInTangentSpace.z = dot(eyevec, Normal);

	TexCoord = in_texCoord;
}