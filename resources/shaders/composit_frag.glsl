// Composit shader, composes the image from lightintensities and albedo

#version 150

uniform sampler2D texture0;
uniform sampler2D texture1;

in vec2 TexCoord;
out vec3 out_Radiance;

void main(void)
{
  vec4 albedoAndNoise = texture(texture0, TexCoord);
  vec3 light = texture(texture1, TexCoord).rgb;

  out_Radiance = light.rgb * albedoAndNoise.rgb;
}