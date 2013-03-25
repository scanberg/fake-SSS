#version 150

uniform mat4 viewMatrix;

uniform vec4 spotlightDir;

in vec3 in_position;
in vec2 in_texCoord;

out vec2 TexCoord;
out vec3 LightDirViewSpace;
 
void main(void)
{
	gl_Position = vec4(in_position, 1.0);
	TexCoord = in_texCoord;
	LightDirViewSpace = normalize(vec3( viewMatrix * vec4(spotlightDir.xyz, 0.0) ));
}