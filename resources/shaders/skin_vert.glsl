#version 150

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 in_position;
in vec3 in_normal;

out float Depth;
out vec3 Normal;
out vec3 ViewDir;
out vec3 NoisePos;

const float n = 0.1;
const float f = 50.0;
 
void main(void)
{
	float d = - (viewMatrix * vec4(in_position,1.0)).z;
	Depth = (d-n)/(f-n);
	gl_Position = projMatrix * viewMatrix * vec4(in_position, 1.0);

	NoisePos = in_position;

	Normal = mat3(viewMatrix) * in_normal;
	ViewDir = ( viewMatrix * vec4(in_position,1.0) ).xyz;
}