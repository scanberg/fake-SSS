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

		//#ifndef __APPLE__
		glewExperimental = GL_TRUE;
		if (GLEW_OK != glewInit())
		{
			logError("GLEW init error");
			return false;
		}
		//#endif

		logErrorsGL();

		logNote("Successfully created OpenGL-window, version %i.%i",
	         glfwGetWindowParam(GLFW_OPENGL_VERSION_MAJOR),
	         glfwGetWindowParam(GLFW_OPENGL_VERSION_MINOR));

		logNote("GLSL-version: %s",glGetString(GL_SHADING_LANGUAGE_VERSION));

		glEnable(GL_DEPTH_TEST);
    	glEnable(GL_CULL_FACE);
    	glfwSwapInterval(0);

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
		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Engine::swapBuffers()
	{
	    glfwSwapBuffers();
	}
}
