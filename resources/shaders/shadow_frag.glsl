#version 150
 
precision highp float; // needed only for version 1.30

uniform mat4 lightToViewMatrix;

uniform sampler2D texture0;

in vec3 Normal;
in vec3 ViewDir;
in vec4 ShadowProj;

out vec4 out_Color;
 
void main(void)
{
	//vec4 shadowCoord = ShadowProj / ShadowProj.w;
	//shadowCoord.z += 0.0005;

	//float lightDist = texture(texture0, shadowCoord.st).z;

	float shadow = 1.0;

	if(ShadowProj.w > 0.0)
	{
		shadow = textureProj(texture0, ShadowProj);
		//shadow = lightDist < shadowCoord.z ? 0.2 : 1.0;
		//shadow = 0.2;
	}

	float depth = -ViewDir.z;
	vec3 viewDir = normalize(ViewDir);
	vec3 normal = normalize(Normal);

	vec2 texCoord = gl_FragCoord.xy / vec2(640,480);

	vec3 light = vec3( shadow );

	out_Color = vec4(light,1.0);
}