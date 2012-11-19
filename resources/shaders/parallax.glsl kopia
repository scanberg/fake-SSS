#VERTEX#
#version 150

uniform mat4 viewMatrix, projMatrix;

uniform vec2 viewRange = vec2(0.1, 100.0);

in vec3 in_position;
in vec3 in_normal;
in vec3 in_tangent;

in vec2 in_texCoord;

out float Depth;

out vec3 Normal;
out vec3 Tangent;
out vec3 ViewVec;

out vec2 TexCoord;

void main()
{
	gl_Position = projMatrix * viewMatrix * vec4(in_position,1.0);
	
	Normal = vec3(viewMatrix * vec4(in_normal,0.0));
	Tangent = vec3(viewMatrix * vec4(in_tangent,0.0));

	vec3 vVec = -vec3(viewMatrix * vec4(in_position,1.0));

	vec3 t = normalize(Tangent);
	vec3 n = normalize(Normal);
	vec3 b = cross(n,t);

	ViewVec.x = dot(vVec,t);
	ViewVec.y = dot(vVec,b);
	ViewVec.z = dot(vVec,n);

	TexCoord = in_texCoord;

	Depth = (vVec.z - viewRange.x)/(viewRange.y - viewRange.x);
}
#END VERTEX#

#FRAGMENT#
#version 150
precision highp float;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float parallaxDepth = 16.0;

in float Depth;
in vec3 Normal;
in vec3 Tangent;
in vec3 ViewVec;
in vec2 TexCoord;

out vec4 outputFrag[2];

vec2 computeParallaxCoord(vec2 texCoord, vec3 viewVec)
{
	float parallax = 0.02;

	float height = textureLod(texture1, texCoord, 0).a;
	float offset = parallax * (2.0 * height - 1.0);
	vec2 parallaxTexCoord = texCoord + offset * viewVec.xy;

	//refined step
	height += textureLod(texture1, parallaxTexCoord, 0).a;
	offset = parallax * (height - 1.0);

	//offset = clamp(offset,0.0,0.1);

	return texCoord + offset * viewVec.xy;
}

vec3 computePomCoord(vec3 texCoord, vec3 viewVec)
{

	//float parallaxDepth = 16.0;

	//vec4  some_tex = texture2DGrad(iSometexture,  someUV, dx, dy); 

	float maxStepCount = 4.0 / clamp( dot(viewVec, vec3(0.0,0.0,-1.0)), 0.1, 1.0);
	/*
		Parallax occlusion mapping ray-traces the height-map to find the intersection
		with the view vector. We step along the view direction until we find a point
		below the height-map.
	*/
	float nSteps = maxStepCount * length(viewVec.xy);

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
	float diff1 = 1.0 - textureLod(texture1, texCoord.xy, 0).a; // texCoord.z = 1.0 initially
	//float diff1 = 1.0 - textureLod(normalMap, texCoord.xy, 0).w;
	if (diff1 > 0.0){
		do {
			texCoord += dir;

			diff0 = diff1;
			diff1 = texCoord.z - textureLod(texture1, texCoord.xy, 0).a;
			//diff1 = texCoord.z - texture2DLod(normalMap, texCoord.xy, 0).w;
		} while (diff1 > 0.0);

		/*
			Refine coordinate by intersecting the view direction with the surface.
			Assume the surface is linear between the last two height-map samples.
		*/
		texCoord.xy += (diff1 / (diff0 - diff1)) * dir.xy;
	}

	return texCoord;
}

void main()
{
	vec3 normal = normalize(Normal);
	vec3 tangent = normalize(Tangent);
	vec3 viewVec = normalize(ViewVec);

	//if(dot(normal,viewVec)<0.0)
	//	discard;

	//vec3 newTexCoord = computePomCoord(vec3(TexCoord,1.0), viewVec);
	vec2 newTexCoord = computeParallaxCoord(TexCoord,viewVec);

	if (newTexCoord.x > 1.0 || newTexCoord.x < 0.0 || newTexCoord.y > 1.0 || newTexCoord.y < 0.0)
		discard;

	vec2 dx = dFdx(TexCoord);
	vec2 dy = dFdy(TexCoord);

	vec4 frag = textureGrad(texture0,newTexCoord.xy,dx,dy);
	//frag = vec4(Depth);// - newTexCoord.z*0.01;

	outputFrag[0] = vec4(frag.rgb,1.0);
}
#END FRAGMENT#