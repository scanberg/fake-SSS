#README

1. LIGHT DEPTH PASS, Render depth values of lights that cast shadows

2. FRONT PASS, Render depth, albedo + noise, normal + specularity

3. LIGHTS PASS, Render all lights into an accumulated buffer using blend(ONE,ONE)

4. HDR BLUR PASS, Blur the image to bleed very intensive pixels, using a gaussian blur

5. TONE MAP PASS, compose the final image from final and blur, then tonemap to 8-bit values


Thanks to Infinite Realities (www.ir-ltd.net) for the head model,
http://www.cgfeedback.com/cgfeedback/showthread.php?t=751

Gaussian blur in glsl using linear sampling
http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/

Scattering approximation
http://http.developer.nvidia.com/GPUGems/gpugems_ch16.html

EXTERNAL DEPENDENCIES:
glfw2: http://www.glfw.org/
glew: http://glew.sourceforge.net/
glm:  http://glm.g-truc.net/
