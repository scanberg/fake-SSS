#include "Types.h"
#include "Geometry.h"
#include "ObjLoader.h"
#include "Tokenizer.h"
#include "Shader.h"
#include "Engine.h"
#include "Camera.h"
#include "Log.h"

void setupModelMatrix(mat4 &mat);

int main()
{
    glen::Engine *engine = new glen::Engine();

    if(!engine->init())
    {
        delete engine;
        return 0;
    }

    float time;
    int timeLoc;

    glen::Camera cam;
    vec3 lookPos(0,2,0);

    cam.setPosition(0,5,10);
    cam.lookAt(&lookPos);

    mat4 modelMatrix, modelViewMatrix;

    glen::Shader shad("resources/shaders/test_vert.glsl", "resources/shaders/test_frag.glsl");

    glen::Geometry g;
    glen::loadObj(g,"resources/meshes/bunny.obj",0.8f);
    g.createStaticBuffers(  shad.getPositionLocation(),
                            shad.getNormalLocation(),
                            shad.getTexCoordLocation());

    timeLoc = shad.getUniformLocation("time");

    while(true)
    {
        engine->clearBuffers();

        cam.setup();

        setupModelMatrix(modelMatrix);
        modelViewMatrix = cam.getViewMatrix() * modelMatrix;

        shad.bind();

        if(timeLoc > -1)
        {
            time = glfwGetTime();
            glUniform1f(timeLoc, time);
        }

        glUniformMatrix4fv(shad.getViewMatrixLocation(), 1, false, glm::value_ptr(modelViewMatrix));
        glUniformMatrix4fv(shad.getProjMatrixLocation(), 1, false, glm::value_ptr(cam.getProjMatrix()));

        /* do the shiet */
        g.draw();

        shad.unbind();

        if(glfwGetKey(GLFW_KEY_SPACE))
            shad.loadAndCompile();

        engine->swapBuffers();
        if(!glfwGetWindowParam(GLFW_OPENED) || glfwGetKey(GLFW_KEY_ESC) || glfwGetKey('Q'))
			break;
    }

    //clean up
    delete engine;

    return 0;
}

void setupModelMatrix(mat4 &mat)
{
    static int oldMouseX=320, oldMouseY=240;
    static vec2 rotAngle;
    int mouseX, mouseY;

    glfwGetMousePos(&mouseX, &mouseY);
    vec2 mouseMove((float)(mouseX-oldMouseX), (float)(mouseY-oldMouseY));

    oldMouseX = mouseX; oldMouseY = mouseY;

    if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT))
    {
        rotAngle += mouseMove * 0.5f;
        rotAngle.y = glm::clamp(rotAngle.y,0.0f,90.0f);
    }

    mat = glm::rotate(mat4(), rotAngle.y, vec3(1,0,0));
    mat = glm::rotate(mat, rotAngle.x, vec3(0,1,0));

}
