#version 150

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float time;
 
in vec3 in_position;
in vec3 in_normal;

out vec3 Normal;
out vec3 ViewDir;
 
void main(void)
{
	Normal = mat3(viewMatrix) * in_normal;
	gl_Position = projMatrix * viewMatrix * vec4(in_position + 0.1 * Normal * sin(time), 1.0);
	ViewDir = vec3(viewMatrix * vec4(in_position, 1.0));
}