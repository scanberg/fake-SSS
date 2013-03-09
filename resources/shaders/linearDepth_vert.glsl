#version 150

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec2 nearfar = vec2(0.1, 100.0);
 
in vec3 in_position;

out float Depth;
 
void main(void)
{
	vec4 viewSpacePos = viewMatrix * vec4(in_position,1.0);
	Depth = (-viewSpacePos.z - nearfar.x)/(nearfar.y - nearfar.x);
	//Depth = -viewSpacePos.z;
	gl_Position = projMatrix * viewSpacePos;
}