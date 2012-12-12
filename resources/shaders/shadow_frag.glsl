#version 150
 
precision highp float; // needed only for version 1.30

uniform sampler2DShadow texture0;

uniform vec3 lightPos;
uniform vec3 lightDir;
uniform float lightFov;

in vec3 Normal;
in vec3 ViewDir;
in vec3 WorldPos;
in vec4 ShadowProj;

out vec4 out_Color;

const float PI = 3.14159265;

float calculateModulation(vec3 ftl, vec3 ldir)
{
	float fov = lightFov * PI / 360.0;
	float criticalAngle = cos( 0.01 * PI / 180.0 );

	float angle = dot(-ftl, ldir);

	return angle > cos(fov) ? clamp(pow(angle + 0.05, 200.0), 0.0, 1.0) : 0.0;
}
 
void main(void)
{
	float lightIntensity = 0.2;

	vec3 fragToLight = lightPos - WorldPos;
	float lightDist = length(fragToLight);

	vec3 fragToLightDir = fragToLight / lightDist;

	vec3 lightDirection = normalize(lightDir);

	float lightModulation = calculateModulation(fragToLightDir, lightDirection);

	if(ShadowProj.w > 0.0)
	{
		lightIntensity += 0.6*textureProj(texture0, ShadowProj) * lightModulation;
	}

	float depth = -ViewDir.z;
	vec3 viewDir = normalize(ViewDir);
	vec3 normal = normalize(Normal);

	vec2 texCoord = gl_FragCoord.xy / vec2(640,480);

	vec3 color = vec3(1.0,0.0,0.0);

	vec3 light = vec3(lightIntensity);

	out_Color = vec4(color*light,1.0);
}