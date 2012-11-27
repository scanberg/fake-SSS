#include "Engine.h"
#include "Log.h"
//#include <GL/glew.h>
//#include <GL/glfw.h>

#define R_BPP 8
#define G_BPP 8
#define B_BPP 8
#define A_BPP 8
#define DEPTH_BPP 32
#define STENCIL_BPP 8

namespace glen
{

	Engine *glen::Engine::instance = NULL;

	Engine::Engine()
	{
		instance = this;
		currentShader = NULL;
	}

	Engine::~Engine()
	{
	}

	bool Engine::init(int width, int height, bool fullscreen)
	{
		glfwInit();

		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
		glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		glfwOpenWindowHint( GLFW_WINDOW_NO_RESIZE, GL_TRUE );

		int fs = fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW;

		if(!glfwOpenWindow(	width,
							height,
							R_BPP, G_BPP, B_BPP, A_BPP,
							DEPTH_BPP, STENCIL_BPP,
							fs))
		{
			logError("could not create GLFW-window");
			return false;
		}

		logErrorsGL();

		/*GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			logError("GLEW init error");
			return false;
		}*/

		logNote("Successfully created OpenGL-window, version %i.%i",
	         glfwGetWindowParam(GLFW_OPENGL_VERSION_MAJOR),
	         glfwGetWindowParam(GLFW_OPENGL_VERSION_MINOR));

		logNote("GLSL-version: %s",glGetString(GL_SHADING_LANGUAGE_VERSION));

	    #ifdef WIN32
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			logError("could not init GLEW");
			return false;
		}
		#endif

		glEnable(GL_DEPTH_TEST);
    	glEnable(GL_CULL_FACE);
    	glfwSwapInterval(1);

		return true;
	}

	void Engine::update()
	{

	}

	void Engine::render()
	{

	}

	void Engine::clearBuffers()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Engine::swapBuffers()
	{
	    glfwSwapBuffers();
	}

	void Engine::createFBO()
	{
		destroyFBO();

	    // generate namespace for the frame buffer, colorbuffer and depthbuffer
	    glGenFramebuffers(1,&fbo);
	    glGenTextures(1,&colorMap);
	    glGenTextures(1,&normalMap);
	    glGenTextures(1,&depthMap);
	    
	    //switch to our fbo so we can bind stuff to it
	    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	    
	    //create the colorbuffer texture and attach it to the frame buffer
	    glBindTexture(GL_TEXTURE_2D, colorMap);
	    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowSize.x, windowSize.y, 0,GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, colorMap, 0);
	    
	    glBindTexture(GL_TEXTURE_2D, normalMap);
	    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, windowSize.x, windowSize.y, 0,GL_RG, GL_FLOAT, NULL);
	    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D, normalMap, 0);
	    
	    glBindTexture(GL_TEXTURE_2D, depthMap);
	    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowSize.x, windowSize.y, 0,GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, depthMap, 0);
	    
	    glBindTexture(GL_TEXTURE_2D, 0);
	    
	    // Go back to regular frame buffer rendering
	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Engine::destroyFBO()
	{
		if(fbo != UNUSED_ADRESS)
	    	glDeleteFramebuffers(1,&fbo);
	    if(colorMap != UNUSED_ADRESS)
	    	glDeleteTextures(1,&colorMap);
	    if(normalMap != UNUSED_ADRESS)
	    	glDeleteTextures(1,&normalMap);
	    if(depthMap != UNUSED_ADRESS)
	    	glDeleteTextures(1,&depthMap);

	    fbo = colorMap = normalMap = depthMap = UNUSED_ADRESS;
	}
}
