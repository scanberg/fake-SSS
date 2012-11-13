#include "Engine.h"
#include "Log.h"
#include <GL/glew.h>
#include <GL/glfw.h>

glen::Engine::Engine()
{

}

glen::Engine::~Engine()
{

}

bool glen::Engine::init(int width, int height, bool fullscreen, int samplesPerPixel)
{
	glfwInit();

	//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR,3);
	//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR,1);

	int fs = fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW;

	if(glfwOpenWindow(width,height,8,8,8,8,32,8,fs)==GL_FALSE)
	{
		logError("could not create GLFW-window");
		return false;
	}

	logNote("Successfully created OpenGL-window, version %i.%i",
         glfwGetWindowParam(GLFW_OPENGL_VERSION_MAJOR),
         glfwGetWindowParam(GLFW_OPENGL_VERSION_MINOR));

    #ifdef WIN32
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		logError("could not init GLEW");
		return false;
	}
	#endif

	return true;
}

void glen::Engine::update()
{

}

void glen::Engine::render()
{

}

void glen::Engine::swapBuffers()
{
    glfwSwapBuffers();
}
