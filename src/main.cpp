#include "Geometry.h"
#include "Log.h"
#include "Tokenizer.h"
#include "Shader.h"
#include "Engine.h"

int main()
{
    glen::Engine *engine = new glen::Engine();

    if(!engine->init())
    {
        delete engine;
        return 0;
    }

    bool running = true;
    while(running)
    {
        engine->swapBuffers();
        if(!glfwGetWindowParam(GLFW_OPENED) || glfwGetKey(GLFW_KEY_ESC))
			running = false;
    }

    delete engine;

    return 0;
}
