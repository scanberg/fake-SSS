#VERTEX#
#version 150

uniform mat4 viewMatrix, projMatrix;

in vec3 in_position;
in vec3 in_normal;
in vec3 in_color;

out vec3 Color;
out vec3 Normal;
out vec3 ViewVec;

void main()
{
	Color = in_color;
	gl_Position = projMatrix * viewMatrix * vec4(in_position,1.0);
	ViewVec = -vec3(viewMatrix * vec4(in_position,1.0));
	Normal = vec3(viewMatrix * vec4(in_normal,0.0));
}
#END VERTEX#

#FRAGMENT#
#version 150
precision highp float;

in vec3 Color;
in vec3 Normal;
in vec3 ViewVec;
out vec4 outputFrag;

void main()
{
	vec3 normal = normalize(Normal);
	vec3 viewvec = normalize(ViewVec);

	float fragment = 1.0-dot(normal,viewvec);

	//if(fragment<0.0)
		//discard;

	outputFrag = vec4(Color+vec3(fragment,fragment,fragment),1.0);
}
#END FRAGMENT#