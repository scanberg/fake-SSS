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

	void setOuterAngle(float angle) { position.w = angle; setInnerAngle(angle - 5.0); }
	void setInnerAngle(float angle) { direction.w = glm::clamp(angle, 0.0f, position.w); }

	void setPosition(vec3 pos) { position.x = pos.x; position.y = pos.y; position.z = pos.z; }
	void setDirection(vec3 dir) { direction = vec4(glm::normalize(dir), direction.w); }
	void setLookAt(vec3 pos) { direction = vec4(glm::normalize(pos-getPosition()), direction.w); }
	void setColor(vec3 col) { color = vec4(col,color.w); }

	void setNear(float near) { position.w = near; }
	void setFar(float far) { direction.w = far; }
	void setLumen(float lm) { color.w = lm; }

	float getOuterAngle() { return position.w; }
	float getInnerAngle() { return direction.w; }
	float getLumen() { return color.w; }

	vec3 getPosition() { return vec3(position); }
	vec3 getDirection() { return vec3(direction); }
	vec3 getColor() { return vec3(color); }

	float getNear() { return near; }
	float getFar() { return far; }

	unsigned int getShadowMap();

	const mat4 &getViewMatrix() { return viewMatrix; }
	const mat4 &getProjMatrix() { return projMatrix; }
	const mat4 &getTextureMatrix() { return textureMatrix; }

private:

	int width, height;
	float near;
	float far;

	// Pack uniform-data to reduce strain on GPU-bus, i hope.
	vec4 position;	// outerAngle packed in w
	vec4 direction;	// innerAngle packed in w
	vec4 color;		// lumen packed in w

	mat4 viewMatrix;
	mat4 projMatrix;
	mat4 textureMatrix;

	Framebuffer2D *depthFbo;
};

#endif