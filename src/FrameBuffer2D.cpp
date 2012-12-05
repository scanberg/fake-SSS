#include <glm/glm.hpp>
#include "Log.h"
#include "FrameBuffer2D.h"

#define clamp(x,min,max) (x < min) ? min : ( (x > max) ? max : x )

FrameBuffer2D::FrameBuffer2D(int numAuxBuffers)
{
	auxBuffersSize = clamp(numAuxBuffers, 0, 4);
}

FrameBuffer2D::~FrameBuffer2D()
{

}

bool FrameBuffer2D::createBuffers()
{

	for(int i=0; i<auxBuffersSize; i++)
	{
		glGenTextures(auxBuffersSize,auxHandle);

	    glBindTexture(GL_TEXTURE_2D, auxHandle[i]);
	    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0,GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	}

    glGenTextures(1,&depthMap);

    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0,GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void FrameBuffer2D::destroyBuffers()
{
	if(glIsFramebuffer(FrameBuffer2DHandle))
    	glDeleteFramebuffers(1,&FrameBuffer2D);

	if(glIsTexture(auxBuffer[0]))
    	glDeleteTextures(1,&auxBuffer[0]);

	if(glIsTexture(auxBuffer[1]))
    	glDeleteTextures(1,&auxBuffer[1]);

	if(glIsTexture(auxBuffer[2]))
    	glDeleteTextures(1,&auxBuffer[2]);

	if(glIsTexture(auxBuffer[3]))
    	glDeleteTextures(1,&auxBuffer[3]);

    if(glIsTexture(stencilBuffer))
    	glDeleteTextures(1,&stencilBuffer);

    if(glIsTexture(depthBuffer))
    	glDeleteTextures(1,&depthBuffer):

}