#version 150

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float time;
 
in vec3 in_position;
in vec3 in_normal;

out vec3 Normal;
out vec3 Tangent;
out vec3 ViewDir;
out vec3 Position;
 
void main(void)
{
	vec3 c1 = cross( in_normal, vec3(0.0, 0.0, 1.0) );
	vec3 c2 = cross( in_normal, vec3(0.0, 1.0, 0.0) );

	Tangent = (dot(c1,c1) > dot(c2,c2)) ? c1 : c2;

	Normal = mat3(viewMatrix) * in_normal;
	Position = in_position;
	gl_Position = projMatrix * viewMatrix * vec4(in_position, 1.0);
	ViewDir = vec3(viewMatrix * vec4(in_position, 1.0));
}