#README

1. LIGHT DEPTH PASS, Render lights that cast shadows 32		512*512

2. BACK FACE PASS, Render back face depth and light, 32 + 3*16
3. FRONT FACE PASS, Render front face depth, albedo, normal and light, 32 + 3*8? + 2*16 + 3*16

4. LIGHT BLUR PASS, Blur front & back light with gaussian kernel, Use mask to stay inside?

5. COMPOSE PASS, Render the correct image with fake SSS, 3x16

6. HDR BLUR PASS, Blur the image to bleed very intensive pixels

7. TONE MAP PASS, tone map that shit to U8 values and display