#VERTEX#
#version 150

uniform mat4 viewMatrix, projMatrix;

in vec3 in_position;
in vec3 in_normal;
in vec3 in_tangent;

in vec2 in_texCoord;

out vec3 Normal;
out vec3 ViewVec;

out vec2 TexCoord;

void main()
{
	gl_Position = projMatrix * viewMatrix * vec4(in_position,1.0);
	ViewVec = -vec3(viewMatrix * vec4(in_position,1.0));
	Normal = vec3(viewMatrix * vec4(in_normal,0.0));

	TexCoord = in_texCoord;   
}
#END VERTEX#

#FRAGMENT#
#version 150
precision highp float;

uniform sampler2D texture0;
uniform sampler2D texture1;

in vec3 Normal;
in vec3 ViewVec;
in vec2 TexCoord;

out vec4 outputFrag;

void main()
{
	vec3 normal = normalize(Normal);
	vec3 viewvec = normalize(ViewVec);

	float fragment = 1.0-dot(normal,viewvec);
	vec4 diffuseMap = texture(texture0,TexCoord);
	vec4 normalMap = texture(texture1,TexCoord);

	outputFrag = vec4(diffuseMap.a,diffuseMap.a,diffuseMap.a,1.0);
}
#END FRAGMENT#