#README

1. LIGHT DEPTH PASS, Render lights that cast shadows

2. FRONT PASS, Render depth, albedo, normal (And world position as a cheat! World pos should be calculated from depth!)

2. LIGHTS PASS, Render all lights into an accumulated buffer using blend(ONE,ONE)

3. LIGHT BLUR PASS, Blur light

4. COMPOSE PASS, Render the correct image from albedo, and light

5. HDR BLUR PASS, Blur the image to bleed very intensive pixels (Perhaps not necessary to have light blur and hdr blur)

6. TONE MAP PASS, tone map that shit to U8 values and display