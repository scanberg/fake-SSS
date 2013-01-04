#version 150

uniform sampler2D texture0;

out vec4 FragmentColor;

uniform float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
uniform float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

uniform float textureSize = 768.0;

void main(void)
{
    FragmentColor = texture( texture0, vec2(gl_FragCoord)/textureSize ) * weight[0];
    for (int i=1; i<3; i++) {
        FragmentColor +=
            texture( texture0, ( vec2(gl_FragCoord)+vec2(0.0, offset[i]) )/textureSize )
                * weight[i];
        FragmentColor +=
            texture( texture0, ( vec2(gl_FragCoord)-vec2(0.0, offset[i]) )/textureSize )
                * weight[i];
    }
}