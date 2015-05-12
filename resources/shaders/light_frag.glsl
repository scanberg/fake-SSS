#version 150
 
precision highp float; // needed only for version 1.30

uniform mat4 viewMatrix;
uniform mat4 invViewMatrix;
uniform mat4 textureMatrix;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform vec4 spotlightPos;
uniform vec4 spotlightDir;
uniform vec4 spotlightColor = vec4(1,1,1,100);
uniform vec2 spotlightNearFar = vec2(0.1,5.0);

uniform vec3 camPos;
uniform vec2 camRange = vec2(0.1,100.0);
uniform float camRatio = 1024.0/768.0;
uniform float camFov = 60.0;

uniform float density = 100.0;

in vec2 TexCoord;
in vec3 LightDirViewSpace;

out vec3 out_frag0;

const float RAD = 3.14159265 / 180.0;
const float HALF_RAD = 3.14159265 / 360.0;
const float ShadowDepthOffset = 0.0008;

// http://olivers.posterous.com/linear-depth-in-glsl-for-real
float linearizeDepth(float d, vec2 nf)
{
  float z_n = 2.0 * d - 1.0;
  return 2.0 * nf.x * nf.y /
  	(nf.y + nf.x - z_n * (nf.y - nf.x));
}

vec3 calcViewSpaceCoords(float d)
{
  float top = tan(camFov * HALF_RAD);
  float right = camRatio * top;
  vec2 adjustProj = vec2(-right, top);

  vec3 viewCoord = vec3((TexCoord.x-0.5) * 2.0, (-TexCoord.y+0.5) * 2.0, -linearizeDepth(d, camRange));

  viewCoord.xy *= viewCoord.z * adjustProj;

  return viewCoord;
}

void main(void)
{
	// set output data to zero
	out_frag0 = vec3(0);

	// get view space coordinates
	vec3 viewCoord = calcViewSpaceCoords(texture(texture0,TexCoord).r);

	// Safe check to check if the fragment really is a part of the scene
	if (viewCoord.z <= -100)
		discard;

	vec3 worldCoord = vec3(invViewMatrix * vec4(viewCoord,1.0));

	vec4 albedoAndNoise = texture(texture1, TexCoord);
	vec3 normalAndSpec = texture(texture2, TexCoord).rgb;

	vec3 viewSpaceNormal = vec3(normalAndSpec.xy, 0.0);
	viewSpaceNormal.z = sqrt(1.0 - dot(viewSpaceNormal.xy, viewSpaceNormal.xy));
	viewSpaceNormal = normalize(viewSpaceNormal);

	vec3 lightDirViewSpace = LightDirViewSpace;

	//vec4 shadowCoord = textureMatrix * vec4(worldCoord, 1.0);

	vec3 lightToFrag = (worldCoord - spotlightPos.xyz);

	float lightToFragDist = length(lightToFrag);
	//lightToFragDist = shadowCoord.z;

	vec3 L = lightToFrag / lightToFragDist;
	vec3 D = spotlightDir.xyz;

	float lightOuterAngle = spotlightPos.w;
	float lightInnerAngle = spotlightDir.w;

	float cur_angle = dot(L,D);
	//cur_angle = dot(normalize(shadowCoord.xyz),vec3(0,0,-1));
	float inner_angle = cos(lightInnerAngle);
	float outer_angle = cos(lightOuterAngle);
	float diff_angle = inner_angle - outer_angle;

	// Soft edge on spotlight
	float spot = clamp((cur_angle - outer_angle) /
					diff_angle, 0.0, 1.0);

	float lightLumen = spotlightColor.a;

	// Light attenuation term
	float att = lightLumen / (lightToFragDist*lightToFragDist);

	float cosTerm = dot(-lightDirViewSpace, viewSpaceNormal);

	// Total spotlight contribution
	vec3 spotLightContrib = spot * att * spotlightColor.rgb;

	vec4 shadowCoord = textureMatrix * vec4(viewCoord, 1.0);
	vec3 coord = shadowCoord.xyz/shadowCoord.w;

	// World depth values
	float textureDepth = texture(texture3, coord.xy).r;

	// Calculate the distance through the material at the fragments location towards the spotlight
	float lightDepth = textureDepth;
	float fragDepthFromLight = coord.z;

	float deltaDepth = max(0.0, fragDepthFromLight - lightDepth);

	// Map density to a sigma term
	float sigma = pow(density*10.0,3.0);
	float scatterTerm = deltaDepth * 1500 * exp((-deltaDepth) * sigma);

	// Should be a uniform based on material type
	vec3 insideColor = vec3(1.0,0.0,0.0);

	// spotLightContrib that is used here is for the front fragment and therefore a cheat,
	// a new contribution should be calculated from the 'back' fragment as seen from the light.
	vec3 subSurfaceContrib = scatterTerm * insideColor * spotLightContrib  * max(0.0, -cosTerm);

	vec3 surfaceContrib = vec3(0.0);
	vec3 specularContrib = vec3(0.0);

	if(textureDepth > (shadowCoord.z - ShadowDepthOffset)/shadowCoord.w)
	{
		// Modify costerm to falloff faster, to avoid flickering due to glancing angles
		surfaceContrib = max(0.0, cosTerm * 1.2 - 0.2 ) * spotLightContrib;

		// Unpack specular base and exponent
		float specularBase = fract(normalAndSpec.z);
		float specularExp = normalAndSpec.z - specularBase;
		specularBase /= 0.99;

		vec3 lightReflection = reflect(lightDirViewSpace, viewSpaceNormal);
		specularContrib = pow(max(0.0, dot(vec3(0,0,1), lightReflection)), specularExp) * specularBase * spotLightContrib;
	}

	const float backLightNoiseWeight = 0.90;
	const float frontLightNoiseWeight = 0.27;

	out_frag0 += (backLightNoiseWeight * albedoAndNoise.a + ( 1.0 - backLightNoiseWeight ) ) * subSurfaceContrib;
	out_frag0 += (frontLightNoiseWeight * albedoAndNoise.a +  ( 1.0 - frontLightNoiseWeight ) ) * surfaceContrib;
	out_frag0 *= albedoAndNoise.rgb;
	out_frag0 += specularContrib;
}