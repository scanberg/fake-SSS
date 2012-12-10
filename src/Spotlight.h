#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "Types.h"
#include "Framebuffer2D.h"

class Spotlight
{
public:
	Spotlight();
	~Spotlight();

	void bindFbo();
	void unbindFbo();

	void setup();

	void setPosition(vec3 pos) { position = vec4(pos, position.w); }
	void setDirection(vec3 dir) { direction = vec4(dir, direction.w); }
	void setColor(vec3 col) { color = vec4(col,color.w); }

	void setNear(float near) { position.w = near; }
	void setFar(float far) { direction.w = far; }
	void setSpotExponent(float spotexp) { direction.w = spotexp; }

	vec3 getPosition() { return vec3(position); }
	vec3 getDirection() { return vec3(direction); }
	vec3 getColor() { return vec3(color); }

	float getNear() { return position.w; }
	float getFar() { return direction.w; }
	float getSpotExponent() { return color.w; }

	unsigned int getShadowMap();

	const mat4 &getViewMatrix() { return viewMatrix; }
	const mat4 &getProjMatrix() { return projMatrix; }
	const mat4 &getModelMatrix() { return modelMatrix; }

	const mat4 &getBiasProjModelMatrix() { return biasProjModelMatrix; }

private:
	int width, height;

	// Pack data to reduce strain on GPU-bus
	vec4 position;	// near-value packed in w
	vec4 direction;	// far-value packed in w
	vec4 color;		// spotexponent packed in w

	float fov;

	mat4 biasProjModelMatrix;
	mat4 viewMatrix;
	mat4 modelMatrix;
	mat4 projMatrix;

	Framebuffer2D *depthFbo;
};

#endif