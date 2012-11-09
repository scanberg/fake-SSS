#VERTEX#
#version 150

uniform mat4 viewMatrix, projMatrix;

in vec3 in_position;
in vec3 in_normal;
in vec3 in_tangent;

in vec2 in_texCoord;

out vec3 Normal;
out vec3 ViewVec;

out vec2 TexCoord;

void main()
{
	gl_Position = projMatrix * viewMatrix * vec4(in_position,1.0);
	ViewVec = -vec3(viewMatrix * vec4(in_position,1.0));
	Normal = vec3(viewMatrix * vec4(in_normal,0.0));

	TexCoord = in_texCoord;   
}
#END VERTEX#

#FRAGMENT#
#version 150
precision highp float;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float parallaxDepth = 16.0;

in vec3 Normal;
in vec3 ViewVec;
in vec2 TexCoord;

out vec4 outputFrag;

vec3 computeCoord(vec3 texCoord, vec3 viewVec)
{

	//vec2 dx = dFdx(texCoord.xy);
	//vec2 dy = dFdy(texCoord.xy);

	//vec4  some_tex = texture2DGrad(iSometexture,  someUV, dx, dy); 

	float maxStepCount = 4.0 / clamp( dot(viewVec, vec3(0.0,0.0,-1.0)), 0.1, 1.0);
	/*
		Parallax occlusion mapping ray-traces the height-map to find the intersection
		with the view vector. We step along the view direction until we find a point
		below the height-map.
	*/
	float nSteps = maxStepCount;

	vec3 dir = viewVec;
	dir.xy /= parallaxDepth;
	dir /= -nSteps * dir.z;

	/*
		First test. Check this separately outside the loop in case the first sample
		is already on the surface (true for large parts of the wood texture in this
		sample), in which case we don't need to enter the loop at all. This saves us
		one texture sample and some computations in this case, plus guarantees that
		we never get a division by zero below.
	*/
	float diff0;
	float diff1 = 1.0 - texture(texture1, texCoord.xy).a; // texCoord.z = 1.0 initially
	//float diff1 = 1.0 - textureLod(normalMap, texCoord.xy, 0).w;
	if (diff1 > 0.0){
		do {
			texCoord += dir;

			diff0 = diff1;
			diff1 = texCoord.z - texture(texture1, texCoord.xy).a;
			//diff1 = texCoord.z - texture2DLod(normalMap, texCoord.xy, 0).w;
		} while (diff1 > 0.0);

		/*
			Refine coordinate by intersecting the view direction with the surface.
			Assume the surface is linear between the last two height-map samples.
		*/
		texCoord.xy += (diff1 / (diff0 - diff1)) * dir.xy;
	}

	return texCoord.xyz;
}
	
void main (void)
{	
	vec3 viewVec = normalize(ViewVec);

	vec3 parallaxTexCoord = computeCoord(vec3(TexCoord.xy,1.0), viewVec);
		
	outputFrag = vec4(texture(texture0, parallaxTexCoord.xy).xyz, 1.0);
	//outputFrag = texture(texture0, TexCoord.xy);

}
#END FRAGMENT#