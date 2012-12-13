#version 150
 
precision highp float; // needed only for version 1.30

uniform sampler2DShadow texture0;

uniform vec3 lightPos;
uniform vec3 lightDir;
uniform float lightFov;
uniform float lightInnerAngle;

in vec3 Normal;
in vec3 ViewDir;
in vec3 LightDir;
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
	float lightIntensity = 0.0;

	float lightDist = length(LightDir);

	vec3 L = normalize(LightDir);
	vec3 D = normalize(lightDir);

	float cur_angle = dot(-L,D);
	float inner_angle = cos(toRadians(lightInnerAngle * 0.5));
	float outer_angle = cos(toRadians(lightFov * 0.5));
	float diff_angle = inner_angle - outer_angle;

	float spot = clamp((cur_angle - outer_angle) /
					diff_angle, 0.0, 1.0);

	if(ShadowProj.w > 0.0)
	{
		lightIntensity += textureProj(texture0, ShadowProj) * spot;
	}


	vec3 color = vec3(1.0,0.0,0.0);
	vec3 light = vec3(lightIntensity);

	out_Color = vec4(color*light,1.0);
}