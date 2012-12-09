#ifndef LIGHT_H
#define LIGHT_H

#include "Types.h"
#include "Framebuffer2D.h"

class Light
{
public:
	Light();
	~Light();
	
private:
	vec3 position;
	vec3 direction;
	vec3 color;

	float radius;

	Framebuffer2D shadowFbo;
};

#endif