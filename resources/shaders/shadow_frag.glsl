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

	float cos_cur_angle = dot(-L,D);
	float cos_inner_angle = cos(toRadians(lightInnerAngle / 2.0));
	float cos_outer_angle = cos(toRadians(lightFov / 2.0));
	float cos_diff = cos_inner_angle - cos_outer_angle;

	float spot = clamp((cos_cur_angle - cos_outer_angle) /
					cos_diff, 0.0, 1.0);

	if(ShadowProj.w > 0.0)
	{
		lightIntensity += textureProj(texture0, ShadowProj) * spot;
	}


	vec3 color = vec3(1.0,0.0,0.0);
	vec3 light = vec3(lightIntensity);

	out_Color = vec4(color*light,1.0);
}