#version 150

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
 
in vec3 in_position;
 
void main(void)
{
	gl_Position = projMatrix * viewMatrix * vec4(in_position, 1.0);
}