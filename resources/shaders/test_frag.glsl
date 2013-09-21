#version 150

uniform sampler2D texture0;
 
in vec2 TexCoord;

out vec4 out_frag0;
 
void main(void)
{
	out_frag0 = texture(texture0, TexCoord);
}