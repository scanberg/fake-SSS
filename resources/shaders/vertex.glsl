// Procedural shading demo, Stefan Gustavson 2011.
// This code is in the public domain.

uniform float time;
uniform sampler2D teximage;
varying vec2 st; // 2D texcoords (from OpenGL texcoords)
varying vec3 stp; // 3D texcoords (from vertex xyz position)

void main( void )
{
  // Get the texture coordinates
  st = gl_MultiTexCoord0.xy;
  stp = gl_Vertex.xyz;

  vec4 displacedVertex = vec4(gl_Vertex.xyz + gl_Normal * 0.1 * sin(20.0*st.t-6.0*time), 1.0);

  gl_Position = gl_ModelViewProjectionMatrix * displacedVertex;
}
