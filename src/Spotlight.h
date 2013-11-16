/**
Copyright (C) 2012-2014 Robin Skånberg

Permission is hereby granted, free of charge,
to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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

	void setOuterAngle(float angle) { position.w = angle; setInnerAngle(angle - 5.0f); }
	void setInnerAngle(float angle) { direction.w = glm::clamp(angle, 0.0f, position.w); }

	void setPosition(vec3 pos) { position.x = pos.x; position.y = pos.y; position.z = pos.z; }
	void setDirection(vec3 dir) { direction = vec4(glm::normalize(dir), direction.w); }
	void setLookAt(vec3 pos) { direction = vec4(glm::normalize(pos-getPosition()), direction.w); }
	void setColor(vec3 col) { color = vec4(col,color.w); }

	void setNear(float n) { near = n; }
	void setFar(float f) { far = f; }
	void setLumen(float lm) { color.w = lm; }

	void setPositionUniform(const char *str);
	void setDirectionUniform(const char *str);
	void setColorUniform(const char *str);

	float getOuterAngle() { return position.w; }
	float getInnerAngle() { return direction.w; }
	float getLumen() { return color.w; }

	vec3 getPosition() { return vec3(position); }
	vec3 getDirection() { return vec3(direction); }
	vec3 getColor() { return vec3(color); }

	const vec4 &getRawPosition() { return position; }
	const vec4 &getRawDirection() { return direction; }
	const vec4 &getRawColor() { return color; }

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

	// Pack uniform-data to reduce strain on GPU-bus, I hope.
	vec4 position;	// outerAngle packed in w
	vec4 direction;	// innerAngle packed in w
	vec4 color;		// lumen packed in w

	mat4 viewMatrix;
	mat4 projMatrix;
	mat4 textureMatrix;

	Framebuffer2D *depthFbo;
};

#endif