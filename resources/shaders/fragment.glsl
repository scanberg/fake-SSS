// Procedural shading demo, Stefan Gustavson 2011.

uniform float time;
uniform sampler2D teximage;
varying vec2 st;
varying vec3 stp;


float aastep ( float threshold , float value ) {
  float afwidth = 0.7 * length ( vec2 ( dFdx ( value ), dFdy ( value )));
  return smoothstep ( threshold - afwidth , threshold + afwidth , value );
}


// Description : Array- and textureless GLSL 2D simplex noise.
// Author : Ian McEwan, Ashima Arts. Version: 20110822
// Copyright (C) 2011 Ashima Arts. All rights reserved.
// Distributed under the MIT License. See LICENSE file.
// https://github.com/ashima/webgl-noise

vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

float snoise(vec2 v) {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i = floor(v + dot(v, C.yy) );
  vec2 x0 = v - i + dot(i, C.xx);
// Other corners
  vec2 i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
                           + i.x + vec3(0.0, i1.x, 1.0 ));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
                          dot(x12.zw,x12.zw)), 0.0);
  m = m*m; m = m*m;
// Gradients
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 a0 = x - floor(x + 0.5);
// Normalise gradients implicitly by scaling m
  m *= 1.792843 - 0.853735 * ( a0*a0 + h*h );
// Compute final noise value at P
  vec3 g;
  g.x = a0.x * x0.x + h.x * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}


void main( void )
{
  vec3 rgb = texture2D(teximage, st).rgb; // RGB image
  float n = snoise(st*50.0); // Fractal noise
  n += 0.5*snoise(st*100.0);
  n += 0.25*snoise(st*200.0);

  vec3 blobs;
  blobs.r = aastep(0.5, rgb.r + n);
  blobs.g = aastep(0.5, rgb.g + n);
  blobs.b = aastep(0.5, rgb.b + n);
  gl_FragColor = vec4(vec3(blobs), 1.0);
}
