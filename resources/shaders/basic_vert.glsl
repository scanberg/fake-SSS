#version 140

uniform mat4 viewMatrix, projMatrix;
in vec3 in_position;

void main()
{
	gl_Position = projMatrix * viewMatrix * vec4(in_position,1.0); 
}