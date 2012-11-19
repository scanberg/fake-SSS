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

    float vert[9]; 
    
    vert[0] = 0.0f; vert[1] = 0.8f; vert[2] =-1.0f;
    vert[3] =-0.8f; vert[4] =-0.8f; vert[5] =-1.0f;
    vert[6] = 0.8f; vert[7] =-0.8f; vert[8]= -1.0f;

    GLuint vbo;
    glGenBuffers(1,&vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9*sizeof(GLfloat), vert, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0); 
    glEnableVertexAttribArray(0);

    GLuint vao;
    glGen

    glen::Shader shad("resources/shaders/basic_vert.glsl", "resources/shaders/basic_frag.glsl");

    /*while(true)
    {
        engine->clearBuffers();
        engine->swapBuffers();

        if(!glfwGetWindowParam(GLFW_OPENED) || glfwGetKey(GLFW_KEY_ESC))
			break;
    }*/

    //clean up
    delete engine;

    return 0;
}
