#version 150

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec2 nearfar = vec2(0.1, 50.0);
 
in vec3 in_position;

out float Depth;
 
void main(void)
{
	float d = -(viewMatrix * vec4(in_position,1.0)).z;
	Depth = (d - nearfar.x)/(nearfar.y - nearfar.x);
	//Depth = d;
	gl_Position = projMatrix * viewMatrix * vec4(in_position, 1.0);
}