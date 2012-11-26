#version 150
 
precision highp float; // needed only for version 1.30
 
in vec3 Normal;
in vec3 ViewDir;
out vec4 out_Color;
 
void main(void)
{
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(ViewDir);

	vec3 color = vec3(1.0, 0.0, 0.0) * max(0.0, dot(-viewDir,normal));
	out_Color = vec4(color, 1.0);
}