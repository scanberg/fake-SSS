#version 150
 
precision highp float; // needed only for version 1.30

uniform int numLights = 1;

uniform sampler2DArrayShadow texture[8];
uniform vec4 lightPos[8];
uniform vec4 lightDir[8];
uniform vec4 lightColor[8];

in vec4 ShadowProj[8];

in vec3 Normal;
in vec3 WorldPos;

out vec4 out_Color;

const float DegToRad = 0.0174532925199432957692369076848861;

void main(void)
{
	vec3 radiance = vec3(0.0);

	for(int i=0; i<numLights; i++)
	{
		vec3 lightToFrag = (WorldPos - lightPos[i].xyz);

		float lightDist = length(lightToFrag);

		vec3 L = lightToFrag / lightDist;
		vec3 D = lightDir[i].xyz;

		float lightOuterAngle = lightPos[i].w;
		float lightInnerAngle = lightDir[i].w;

		float cur_angle = dot(-L,D);
		float inner_angle = cos(lightInnerAngle * 0.5 * DegToRad);
		float outer_angle = cos(lightOuterAngle * 0.5 * DegToRad);
		float diff_angle = inner_angle - outer_angle;

		// Soft edge on spotlight
		float spot = clamp((cur_angle - outer_angle) /
						diff_angle, 0.0, 1.0);

		float lightLumen = lightColor[i].a;

		// Light attenuation term
		float att = lightLumen / (lightDist*lightDist);

		if(ShadowProj[i].w > 0.0)
		{
			radiance += textureProj(texture[i], ShadowProj[i]) * spot * att * lightColor[i].rgb;
		}
	}

	out_Color = vec4(radiance,1.0);
}