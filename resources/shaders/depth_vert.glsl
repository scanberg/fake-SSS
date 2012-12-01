#version 150

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
 
in vec3 in_position;

out float Depth;

const float n = 0.1;
const float f = 50.0;
 
void main(void)
{
	float d = - (viewMatrix * vec4(in_position,1.0)).z;
	Depth = (d-n)/(f-n);
	gl_Position = projMatrix * viewMatrix * vec4(in_position, 1.0);
}