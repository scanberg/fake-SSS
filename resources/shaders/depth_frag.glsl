#version 150
 
precision highp float; // needed only for version 1.30
 
in float Depth;
 
void main(void)
{
	gl_FragDepth = Depth;
}