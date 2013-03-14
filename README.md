#README

1. LIGHT DEPTH PASS, Render lights that cast shadows 32	bit depth?

2. FRONT PASS, Render depth, albedo, normal , 32 + 3*8? + 2*16

2. LIGHTS PASS, Render all lights into an accumulated buffer

3. LIGHT BLUR PASS, Blur front

4. COMPOSE PASS, Render the correct image with fake SSS, 3x16

5. HDR BLUR PASS, Blur the image to bleed very intensive pixels

6. TONE MAP PASS, tone map that shit to U8 values and display