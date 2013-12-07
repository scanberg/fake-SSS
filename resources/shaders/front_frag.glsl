#version 150

//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
// 

vec2 mod289(vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
  return mod289(((x*34.0)+1.0)*x);
}

vec4 permute(vec4 x) {
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}


float snoise(vec2 v)
  {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
    + i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

float snoise(vec3 v)
{ 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  i = mod289(i); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
}

uniform sampler2D texture0; 		// Diffuse color map
uniform sampler2D texture1;			// Normal map
uniform sampler2D texture2;     // Specular map

uniform vec3 noiseScale = vec3(20,5,20);
uniform float specularExp = 20.0;

uniform float time = 0.0;

uniform float rainAmount = 0.2;

in vec3 Position;
in vec3 Normal;
in vec3 Tangent;
in vec3 WorldPos;
in vec3 WorldNormal;
in vec2 TexCoord;
in vec3 ViewSpaceCoord;
in vec3 CameraInObjectSpace;
in vec3 EyeInTangentSpace;

out vec4 out_frag0;
out vec3 out_frag1;

float sampleNoise( vec3 coord ) {

  float n = 0.0;

  coord *= noiseScale;

  n += 0.5   * abs( snoise( coord * 2.0 ) );
  n += 0.25  * abs( snoise( coord * 4.0 ) );

  return n;

}

float n_snoise(vec2 coord)
{
  return 0.5 + 0.5*snoise(coord);
}

float n_snoise(vec3 coord)
{
  return 0.5 + 0.5*snoise(coord);
}

float specularBaseNoise( vec2 uv, vec3 worldNormal )
{
  // One cycle is 4 seconds
  const float timeScale = 0.25;
  float t = fract(time*timeScale);

  float w1 = 2.0 * abs(0.5 - t);
  float w2 = 1.0 - w1;

  float s1 = fract(t+0.5);
  float s2 = t;

  vec2 seed1 = uv*50.0 + vec2(0,s1);
  vec2 seed2 = uv*50.0 + vec2(289.0*0.5) + vec2(0,s2);

  float n1 = n_snoise( seed1 );
  float n2 = n_snoise( seed2 );

  float n = n1 * w1 + n2 * w2;

  vec2 scale = vec2(80.0,10.0);

  float noise = snoise(vec3(uv * scale + vec2(0,time*1.0), time*0.2));

  n = smoothstep(0.2, 1.0, noise);

  //float verticalComponent = 0.3 + 0.7*(1.0 - 1.0*abs(dot(worldNormal, vec3(0,1,0))));

  return n;
}

float dropNoise(vec2 uv)
{
  vec2 rainDropScale = vec2(50.0,25.0) * (0.5 + rainAmount);
  float rainDropLife = 1.5*rainAmount;
  float minLimit = 0.8 - rainAmount * 0.25;

  float n = smoothstep(minLimit, 1.0, snoise(vec3((uv + vec2(0,time*0.03)) * rainDropScale ,time * rainDropLife)));

  return n;
}

float flowNoise(vec2 uv)
{
  const vec2 flowScale = vec2(120.0, 5.0);
  const float flowSpeed = 0.8;
  const float flowTime = 0.3;

  float n = smoothstep(0.3, 1.0, snoise(vec3(uv * flowScale + vec2(0,time*flowSpeed), time*flowTime)));

  return n;
}

float waterNoise(vec2 uv)
{
  if(rainAmount == 0.0)
    return 0.0;
    
  float dropScale = 0.5 + rainAmount*0.5;
  float flowScale = rainAmount * 0.5;
  return clamp(dropNoise(uv) * dropScale + flowNoise(uv) * flowScale, 0.0, 1.0);
}

void main(void)
{
  vec2 texCoord = TexCoord;
  float waterHeight = 0.0;
  vec3 waterNormal = vec3(0.0);

  if(rainAmount > 0.0)
  {
  waterHeight = waterNoise(texCoord);

    if(waterHeight > 0.00)
    {
      // Calculate waterdrop normals from height value;
      const vec2 texelSize = vec2(1.0/1024.0);
      float sample[2];
      sample[0] = waterNoise( texCoord + texelSize * vec2(0,1) );
      sample[1] = waterNoise( texCoord + texelSize * vec2(1,0) );
      
      waterNormal.x = waterHeight - sample[1];
      waterNormal.y = waterHeight - sample[0];

      waterNormal.z = 1.0 - sqrt(dot(waterNormal.xy, waterNormal.xy));

      vec3 eye = normalize(-EyeInTangentSpace);
      vec2 offsetDir = eye.xy;

      // N1 = Air, N2 = Water
      const float N1_over_N2 = 1.0 / 1.31;
      const float waterOffsetScale = 0.015;

      // new offset coordinate calculated using snell's law
      texCoord += waterHeight * waterOffsetScale * N1_over_N2 * offsetDir;

    }
  }

  vec3 colorMap = texture(texture0, texCoord).rgb;
  vec3 normalMap = texture(texture1, texCoord).rgb;
  float specularMap = texture(texture2, texCoord).r;

	// Transform into [-1, 1]
	vec3 textureNormal = normalMap * 2.0 - 1.0;

  textureNormal += waterNormal;

  textureNormal = normalize(textureNormal);

	vec3 n = normalize(Normal);
	vec3 t = normalize(Tangent);
	vec3 b = cross(n,t);

	vec3 viewSpaceNormal = normalize(textureNormal.x * t + textureNormal.y * b + textureNormal.z * n);

	const int SAMPLES = 3;
	const float WEIGHT_SCALE = 0.5;
	const float stepsize = 0.01;

	vec3 direction = normalize(Position - CameraInObjectSpace);

	float noise = 0.0;
	float weight = 0.7;

	// Start one step into the material
	vec3 sampleCoord = Position + stepsize * direction;

	for(int i=0; i<SAMPLES; ++i)
	{
		noise += weight * sampleNoise( sampleCoord );
		weight *= WEIGHT_SCALE;
		sampleCoord += stepsize * direction;
	}

  //vec3 worldNormal = normalize(WorldNormal);

  float specBase = 0.2 + waterHeight * 0.8;
  float specularExp = 50.0 + waterHeight*50.0;

  //colorMap *= 1.0 - 0.2 * smoothstep(0.1, 0.3, waterHeight);

  // Pack specular base & exp into one float
  float flooredExp = specularExp - fract(specularExp);
  float specularity = clamp(specBase, 0.0, 1.0) * 0.99 + flooredExp;

	out_frag0 = vec4(colorMap, noise);
	out_frag1 = vec3(viewSpaceNormal.xy, specularity);
}