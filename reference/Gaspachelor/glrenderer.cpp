#include "GLRenderer.h"

GLRenderer* GLRenderer::context = NULL;

GLRenderer::GLRenderer()
{

}

GLRenderer::~GLRenderer()
{

}

GLRenderer *GLRenderer::getContext()
{
	if(!context)
		context = new GLRenderer();

	return context;
}

void GLRenderer::init(int width, int height, bool fullscreen, int fsaa)
{
	glfwInit();

	GLint glfwFullscreen = GLFW_WINDOW;

	if(fullscreen)
		glfwFullscreen = GLFW_FULLSCREEN;

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, fsaa);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	//glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	if(!glfwOpenWindow(width,height,8,8,8,8,32,0,glfwFullscreen))
	{
		glfwTerminate();
	}
	glfwDisable(GLFW_MOUSE_CURSOR);
	glfwSetMousePos(width/2, height/2);
	glfwSwapInterval(1);

	glEnable(GL_DEPTH_TEST);
    glClearColor(1.0,1.0,1.0,1.0);

	ilInit();
	iluInit();

	screenSize.x = width;
	screenSize.y = height;
}

void GLRenderer::terminate()
{
	glfwTerminate();

	delete context;
}

void GLRenderer::clearBuffers()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void GLRenderer::swapBuffers()
{
	glfwSwapBuffers();
}

void GLRenderer::DeferredFBO::init(const glm::ivec2 &screenSize)
{
	destroy();

	glGenFramebuffers(1,&fbo);
	glGenTextures(1,&colorMap);
	glGenTextures(1,&normalMap);
	glGenTextures(1,&depthMap);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glBindTexture(GL_TEXTURE_2D, colorMap);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, screenSize.x, screenSize.y,
		0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, colorMap, 0);

	glBindTexture(GL_TEXTURE_2D, normalMap);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, screenSize.x, screenSize.y,
		0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, normalMap, 0);

	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenSize.x, screenSize.y,
		0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, depthMap, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLRenderer::DeferredFBO::destroy()
{
	glDeleteFramebuffers(1,&fbo);
	glDeleteTextures(1,&colorMap);
	glDeleteTextures(1,&normalMap);
	glDeleteTextures(1,&depthMap);
}