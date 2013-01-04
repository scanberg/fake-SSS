#version 150
 
uniform sampler2DShadow texture0;	// ShadowMap
uniform sampler2D texture1; 		// Diffuse color map
uniform sampler2D texture2;			// Normal map
uniform vec4 spotlightPos;
uniform vec4 spotlightDir;
uniform vec4 spotlightColor = vec4(1,1,1,100);

in vec4 ShadowProj;

in vec3 Normal;
in vec3 WorldPos;
in vec2 TexCoord;

out vec3 out_Color;

const float DegToRad = 3.141592653589793238 / 180.0;

void main(void)
{
	vec3 radiance = vec3(0.0);

	if(ShadowProj.w > 0.0)
	{
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

		vec3 coord = ShadowProj.xyz/ShadowProj.w;
		radiance += texture(texture0, coord) * spot * att * spotlightColor.rgb;
	}

	out_Color = texture(texture1,TexCoord);
}