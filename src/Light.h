#ifndef LIGHT_H
#define LIGHT_H

#include "Types.h"

class Light
{
public:
	Light();
	~Light();
	
	void renderShadowMap();
private:
	ivec2 resolution;

	mat4 projMatrix;
	mat4 modelMatrix;

	u32 fbo;
	u32 shadowMap;

	static Shader shadowMapShader;
};

#endif