#version 150
 
precision highp float; // needed only for version 1.30

uniform sampler2D texture0;

in vec2 TexCoord;

out vec4 out_Color;
 
void main(void)
{
	vec4 textureColor = vec4(texture(texture0, TexCoord).r);
	out_Color = textureColor;
}