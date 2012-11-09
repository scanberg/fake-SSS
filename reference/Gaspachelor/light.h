#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/glfw.h>
#include "boundingbox.h"

class Light
{
private:
	static bool inited;
	static GLuint lightVolumeVBO[4];
	typedef enum
	{
		LIGHT_SPHERE = 0,
		LIGHT_CONE = 1,
		LIGHT_BOXCONE = 2,
		LIGHT_LINE = 3
	}LightType;

protected:
	glm::vec3 color;
	float radius;
	BoundingBox box;

	glm::mat4 modelView;
public:
	void draw();
};

#endif