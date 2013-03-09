#version 150
 
precision highp float; // needed only for version 1.30

uniform mat4 textureMatrix;

uniform sampler2D texture0;
uniform sampler2D texture1;
//uniform sampler2DShadow texture1;

uniform vec4 spotlightPos;
uniform vec4 spotlightDir;
uniform vec4 spotlightColor = vec4(1,1,1,100);
uniform vec2 spotlightNearFar = vec2(0.1,10.0);

// For calculation worldPos from depth.
uniform vec3 cameraPos;
uniform vec3 cameraDir;
uniform vec3 cameraNearFarFov;

in vec2 TexCoord;

out vec3 out_Radiance;

const float DegToRad = 3.141592653589793238 / 180.0;

const float ShadowDepthOffset = 0.0009;

void main(void)
{
	vec3 worldPos = texture(texture0, TexCoord).rgb;

	vec4 shadowProj = textureMatrix * vec4(worldPos, 1.0);
	vec3 coord = shadowProj.xyz/shadowProj.w;

	//coord.z = coord.z * 100.0;

	vec3 radiance = vec3(0.0);

	vec3 lightToFrag = (worldPos - spotlightPos.xyz);

	float lightToFragDist = length(lightToFrag);
	//lightToFragDist = LinearizeDepth(coord.z);

	vec3 L = lightToFrag / lightToFragDist;
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
	float att = lightLumen / (lightToFragDist*lightToFragDist);

	//if(shadowProj.w > 0.0)
	//{
		//radiance += textureProj(texture1, ShadowProj) * spot * att * spotlightColor.rgb;
		//vec3 coord = shadowProj.xyz/shadowProj.w;
		//radiance += texture(texture1, coord) * spot * att * spotlightColor.rgb;

		float lightDepth = texture(texture1, coord.xy).r;
		float diff = max(0.0, (coord.z - lightDepth) * (spotlightNearFar.y - spotlightNearFar.x) + ShadowDepthOffset);

		float sigma_t = 60.0;
		float lightContrib = (diff > 0.0) ? 0.0 : 1.0;
		float subsurfaceContrib = exp(-diff * sigma_t);

		radiance += subsurfaceContrib * spot * att * spotlightColor.rgb;
	//}

	out_Radiance = vec3(radiance);
}