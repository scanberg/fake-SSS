#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "Types.h"
#include "Framebuffer2D.h"
#include "Shader.h"

class Spotlight
{
public:
	Spotlight();
	~Spotlight();

	void bindFbo();
	void unbindFbo();

	void setup();

	void setUniforms(Shader &shader);

	void setFov(float f) { fov = f; setInnerAngle(fov-5.0); }
	void setInnerAngle(float angle) { innerAngle = glm::clamp(angle, 0.0f, fov); }

	void setPosition(vec3 pos) { position.x = pos.x; position.y = pos.y; position.z = pos.z; }
	void setDirection(vec3 dir) { direction.x = dir.x; direction.y = dir.y; direction.z = dir.z; }
	void setLookAt(vec3 pos) { direction = vec4(glm::normalize(pos-getPosition()), direction.w); }
	void setColor(vec3 col) { color = vec4(col,color.w); }

	void setNear(float near) { position.w = near; }
	void setFar(float far) { direction.w = far; }
	void setSpotExponent(float spotexp) { direction.w = spotexp; }

	float getFov() { return fov; }

	vec3 getPosition() { return vec3(position); }
	vec3 getDirection() { return vec3(direction); }
	vec3 getColor() { return vec3(color); }

	float getNear() { return position.w; }
	float getFar() { return direction.w; }
	float getSpotExponent() { return color.w; }

	unsigned int getShadowMap();

	const mat4 &getViewMatrix() { return viewMatrix; }
	const mat4 &getProjMatrix() { return projMatrix; }

	const mat4 &getTextureMatrix() { return textureMatrix; }

private:

	int width, height;
	float fov;
	float innerAngle;

	// Pack data to reduce strain on GPU-bus
	vec4 position;	// near-value packed in w
	vec4 direction;	// far-value packed in w
	vec4 color;		// spotexponent packed in w

	mat4 viewMatrix;
	mat4 projMatrix;
	mat4 textureMatrix;

	Framebuffer2D *depthFbo;
};

#endif