#version 150
 
precision highp float; // needed only for version 1.30

uniform int numLights = 1;

uniform sampler2DShadow texture0;
uniform vec4 spotlightPos;
uniform vec4 spotlightDir;
uniform vec4 spotlightColor = vec4(1,1,1,100);

in vec4 ShadowProj[4];

in vec3 Normal;
in vec3 WorldPos;

out vec4 out_Color;

const float DegToRad = 3.141592653589793238 / 180.0;

void main(void)
{
	vec3 radiance = vec3(0.0);

	vec3 lightToFrag = (WorldPos - spotlightPos.xyz);

	float lightDist = length(lightToFrag);

	vec3 L = lightToFrag / lightDist;
	vec3 D = spotlightDir.xyz;

	float lightOuterAngle = spotlightPos.w;
	float lightInnerAngle = spotlightDir.w;

	float cur_angle = dot(L,D);
	float inner_angle = cos(lightInnerAngle * 0.5 * DegToRad);
	float outer_angle = cos(lightOuterAngle * 0.5 * DegToRad);
	float diff_angle = inner_angle - outer_angle;

	// Soft edge on spotlight
	float spot = clamp((cur_angle - outer_angle) /
					diff_angle, 0.0, 1.0);

	float lightLumen = spotlightColor.a;

	// Light attenuation term
	float att = lightLumen / (lightDist*lightDist);

	if(ShadowProj[0].w > 0.0)
	{
		//radiance += textureProj(texture0, ShadowProj) * spot * att * spotlightColor.rgb;
		vec3 coord = ShadowProj[0].xyz/ShadowProj[0].w;
		radiance += texture(texture0, coord) * spot * att * spotlightColor.rgb;
	}

	out_Color = vec4(radiance,1.0);
}