#include "Geometry.h"
#include "ObjLoader.h"
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

//    GLuint vao;

    glen::Geometry g;
    glen::loadObj(g,"resources/meshes/bunny.obj",1.0f);

    glen::Shader shad("resources/shaders/basic_vert.glsl", "resources/shaders/basic_frag.glsl");

    while(true)
    {
        engine->clearBuffers();
        engine->swapBuffers();

        if(!glfwGetWindowParam(GLFW_OPENED) || glfwGetKey(GLFW_KEY_ESC))
			break;
    }

    //clean up
    delete engine;

    return 0;
}
