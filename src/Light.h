#ifndef LIGHT_H
#define LIGHT_H

#include "Types.h"

class Light
{
public:
	Light() : position(vec4(0,0,0,1)), color(1,1,1,10) {};
	~Light();

	void setPosition(vec3 pos) { position = vec4(pos, position.w); }
	void setColor(vec3 col) { color = vec4(col,color.w); }

	void setRadius(float r) { position.w = r; }
	void setLumen(float lm) { color.w = lm; }

	float getRadius() { return position.w; }
	float getLumen() { return color.w; }

	vec3 getPosition() { return vec3(position); }
	vec3 getColor() { return vec3(color); }

	const vec4 &getRawPosition() { return position; }
	const vec4 &getRawColor() { return color; }

private:

	// Pack uniform-data to reduce strain on GPU-bus, I hope.
	vec4 position;	// outerAngle packed in w
	vec4 color;		// lumen packed in w
};

#endif