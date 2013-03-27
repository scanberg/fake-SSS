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

#include "Engine.h"
#include "Log.h"
//#include <GL/glew.h>
//#include <GL/glfw.h>

#define R_BPP 8
#define G_BPP 8
#define B_BPP 8
#define A_BPP 8
#define DEPTH_BPP 32
#define STENCIL_BPP 0

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
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
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
