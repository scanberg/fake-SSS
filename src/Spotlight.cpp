#include "Spotlight.h"
#include "Log.h"

Spotlight::Spotlight()
{
	width = height = 256;
	fov = 45.0;
	position = vec4(0,0,0,0.1);
	direction = vec4(0,0,-1,50.0);
	color = vec4(1,1,1,10.0);

	depthFbo = NULL;
	depthFbo = new Framebuffer2D(width, height, 0);

	depthFbo->attachBuffer(	FBO_DEPTH,
							GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT,
							GL_LINEAR, GL_LINEAR);
}

Spotlight::~Spotlight()
{
	if(depthFbo)
		delete depthFbo;
}

void Spotlight::bindFbo()
{
	if(depthFbo)
		depthFbo->bind();
}

void Spotlight::unbindFbo()
{
	if(depthFbo)
		depthFbo->unbind();
}

void Spotlight::setup()
{

}

unsigned int Spotlight::getShadowMap()
{
	if(depthFbo)
		return depthFbo->getBufferHandle(FBO_DEPTH);

	return 0;
}

void Spotlight::calcProjMatrix()
{
	float ratio = (float)width / (float)height;
	projMatrix = glm::perspective(fov, ratio, getNear(), getFar());

	calcTextureToWorldMatrix();
}

void Spotlight::calcViewMatrix()
{
	vec3 pos = getPosition();
	vec3 dir = getDirection();

	viewMatrix = glm::lookAt(pos, pos+dir, vec3(0,1,0));

	calcTextureToWorldMatrix();
}

void Spotlight::calcTextureToWorldMatrix()
{
	textureToWorldMatrix = glm::inverse(projMatrix * viewMatrix);
}