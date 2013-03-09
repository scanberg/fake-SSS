#version 150

uniform sampler2D texture0;

in vec2 TexCoord;

out vec4 FragmentColor;

uniform float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
uniform float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

uniform float width = 1024.0;
uniform float height = 768.0;

void main(void)
{
    FragmentColor = texture( texture0, TexCoord ) * weight[0];
    for (int i=1; i<3; i++) {
        FragmentColor +=
            texture( texture0, TexCoord + vec2(0.0, offset[i])/height )
                * weight[i];
        FragmentColor +=
            texture( texture0, TexCoord - vec2(0.0, offset[i])/height )
                * weight[i];
    }
}