#include "Spotlight.h"
#include "Log.h"
#include "Shader.h"

static const mat4 biasMatrix(	0.5, 0.0, 0.0, 0.0,
								0.0, 0.5, 0.0, 0.0,
								0.0, 0.0, 0.5, 0.0,
								0.5, 0.5, 0.5, 1.0);

Spotlight::Spotlight()
{
	width = height = 512;
	near = 0.1;
	far = 100.0;
	position = 	vec4(0,0,0,45.0);
	direction = vec4(0,0,-1,40.0);
	color = 	vec4(1,1,1,100.0);

	depthFbo = NULL;
	depthFbo = new Framebuffer2D(width, height, 0);

	depthFbo->attachBuffer(	FBO_DEPTH,
							GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT,
							GL_LINEAR, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, getShadowMap());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);
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
	float ratio = (float)width / (float)height;
	projMatrix = glm::perspective(getOuterAngle(), ratio, getNear(), getFar());

	vec3 pos = getPosition();
	vec3 dir = getDirection();

	viewMatrix = glm::lookAt(pos, pos+dir, vec3(0,1,0));

	textureMatrix = biasMatrix * projMatrix * viewMatrix;
}

void Spotlight::setPositionUniform(const char *str)
{
    int lightPosLoc = Shader::getBoundShader()->getUniformLocation(str);
    if(lightPosLoc > -1)
    {
        glUniform4fv(lightPosLoc, 1, glm::value_ptr(position));
    }
}

void Spotlight::setDirectionUniform(const char *str)
{
    int lightDirLoc = Shader::getBoundShader()->getUniformLocation(str);
    if(lightDirLoc > -1)
    {
        glUniform4fv(lightDirLoc, 1, glm::value_ptr(direction));
    }
}

void Spotlight::setColorUniform(const char *str)
{
    int lightColorLoc = Shader::getBoundShader()->getUniformLocation(str);
    if(lightColorLoc > -1)
        glUniform4fv(lightColorLoc, 1, glm::value_ptr(color));
}

unsigned int Spotlight::getShadowMap()
{
	if(depthFbo)
		return depthFbo->getBufferHandle(FBO_DEPTH);

	return 0;
}