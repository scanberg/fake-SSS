// Horizontal gaussian blur shader with linear sampling
// Credits to Daniel Rákos
// http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/

#version 150

uniform sampler2D texture0;

in vec2 TexCoord;

out vec3 out_frag0;

uniform float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
uniform float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

uniform float width = 1024.0;
uniform float height = 768.0;

void main(void)
{
    out_frag0 = texture( texture0, TexCoord ).rgb * weight[0];
    for (int i=1; i<3; i++) {
        out_frag0 +=
            texture( texture0, TexCoord + vec2(offset[i], 0.0)/width ).rgb
                * weight[i];
        out_frag0 +=
            texture( texture0, TexCoord - vec2(offset[i], 0.0)/width ).rgb
                * weight[i];
    }
}