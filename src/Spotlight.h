#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "Types.h"
#include "Framebuffer2D.h"

class Spotlight
{
public:
	Spotlight(float fov, float near, float far);
	~Spotlight();

	void setPosition(vec3 pos) { position = vec4(pos, position.w); }
	void setDirection(vec3 dir) { direction = vec4(dir, direction.w); }

	void setRadius(float rad) { position.w = rad; }
	void setSpotExponent(float spotexp) { direction.w = spotexp; }

	void setColor(vec3 col) { color = col; }

	vec3 getPosition() { return vec3(position); }
	vec3 getDirection() { return vec3(direction); }
	vec3 getColor() { return color; }

	float getRadius() { return position.w; }
	float getSpotExponent() { return direction.w; }

	const mat4 &getBiasProjModelMatrix() { return biasProjModelMatrix; }

private:
	// Pack data to reduce strain on GPU-bus
	vec4 position;	// Radius of light packed in position.w
	vec4 direction;	// Spotlight exponent packed in direction.w
	vec3 color;

	float fov;

	mat4 biasProjModelMatrix;

	Framebuffer2D shadowFbo;
};

#endif