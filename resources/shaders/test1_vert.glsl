#version 150

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
 
in vec3 in_position;

out vec4 ShadowProj;
out vec3 WorldPos;
out float Depth;
 
void main(void)
{
	WorldPos = (modelMatrix * vec4(in_position, 1.0)).xyz;
	gl_Position = projMatrix * viewMatrix * vec4(WorldPos, 1.0);
	Depth = -(viewMatrix * vec4(WorldPos,1.0)).z;
	ShadowProj = textureMatrix * (in_position, 1.0);
}