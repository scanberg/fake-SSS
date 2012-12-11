#version 150
 
precision highp float; // needed only for version 1.30

uniform mat4 lightToViewMatrix;

uniform sampler2D texture0;

in vec3 Normal;
in vec3 ViewDir;

out vec4 out_Color;

const float n = 0.1;
const float f = 50.0;

float linearizeDepth(sampler2D tex, vec2 tc)
{
  return (2.0 * n) / (f + n - texture( tex, tc ).r * (f - n));
}

const int width 512;
const int height 512;
 
void main(void)
{
  float depth = -ViewDir.z;
	vec3 viewDir = normalize(ViewDir);
	vec3 normal = normalize(Normal);

	vec2 texCoord = gl_FragCoord.xy / vec2(640,480);

  vec3 light = vec3( 1,0,0 );

	out_Color = vec4(light,1.0);
}