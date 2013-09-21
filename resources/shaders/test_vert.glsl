#version 150

in vec3 in_position;
in vec3 in_texCoord;

out vec2 TexCoord;
 
void main(void)
{
	gl_Position = vec4(in_position, 0.0);
	TexCoord = in_position.xy * 0.5 + vec2(0.5);
}