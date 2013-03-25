// Composit shader, composes the image from lightintensities and albedo

#version 150

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

in vec2 TexCoord;

out vec3 out_Radiance;

vec3 colorMap(float val)
{
  const vec3 upperColor = vec3(0.0,0.0,0.5);
  const vec3 lowerColor = vec3(0.3,0.0,0.0);

  return mix(lowerColor, upperColor, val);
}

void main(void)
{
  vec4 albedoAndNoise = texture(texture0, TexCoord);
  vec3 frontLight = texture(texture1, TexCoord).rgb;
  vec3 backLight = texture(texture2, TexCoord).rgb;
  vec3 specularLight = texture(texture3, TexCoord).rgb;

  const float backLightNoiseWeight = 0.95;
  const float frontLightNoiseWeight = 0.27;

  backLight *= 1.0;

  out_Radiance = vec3(0.0);

  out_Radiance += (backLightNoiseWeight * albedoAndNoise.a + ( 1.0 - backLightNoiseWeight ) ) * backLight;
  out_Radiance += (frontLightNoiseWeight * albedoAndNoise.a +  ( 1.0 - frontLightNoiseWeight ) ) * frontLight;
  out_Radiance *= albedoAndNoise.rgb;

  out_Radiance += specularLight;
}