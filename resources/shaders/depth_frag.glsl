#version 150
 
precision highp float; // needed only for version 1.30
 
in float Depth;
out vec4 out_Color;
 
void main(void)
{
	gl_FragDepth = Depth;
	out_Color = vec4(Depth, 0.0, 0.0, 1.0); 
}