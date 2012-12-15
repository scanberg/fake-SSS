#version 150
 
precision highp float; // needed only for version 1.30

uniform sampler2DShadow texture0;

uniform vec3 lightPos;
uniform vec3 lightDir;
uniform float lightOuterAngle;
uniform float lightInnerAngle;

in vec3 Normal;
in vec3 ViewDir;
in vec3 WorldPos;
in vec4 ShadowProj;

out vec4 out_Color;

const float PI = 3.14159265;

float toRadians(float angle)
{
	return angle * PI / 180.0;
}
 
void main(void)
{
	vec3 fragToLight = (lightPos - WorldPos);
	float lightIntensity = 0.0;

	float lightDist = length(fragToLight);

	vec3 L = fragToLight / lightDist;
	vec3 D = lightDir;

	float cur_angle = dot(-L,D);
	float inner_angle = cos(toRadians(lightInnerAngle * 0.5));
	float outer_angle = cos(toRadians(lightOuterAngle * 0.5));
	float diff_angle = inner_angle - outer_angle;

	// Soft edge on spotlight
	float spot = clamp((cur_angle - outer_angle) /
					diff_angle, 0.0, 1.0);

	// Light attenuation term
	float att = 60.0 / (lightDist*lightDist);

	if(ShadowProj.w > 0.0)
	{
		lightIntensity += textureProj(texture0, ShadowProj) * spot * att;
	}

	vec3 color = vec3(1.0,0.0,0.0);
	vec3 light = vec3(pow(lightIntensity,0.45));

	out_Color = vec4(color * light,1.0);
}