#version 150
 
precision highp float; // needed only for version 1.30

uniform sampler2D texture0;

out vec3 outFrag[2];

in vec4 ShadowProj;
in vec3 WorldPos;
in float Depth;
 
void main(void)
{
	vec3 coord = ShadowProj.xyz/ShadowProj.w;

	float shadow = texture(texture0, coord.xy) > Depth ? 1.0 : 0.0;

	outFrag[0] = vec3(shadow,0,0);
	outFrag[1] = vec3(0,1,0);
	//outFrag[1] = vec3(0,1,0);
}