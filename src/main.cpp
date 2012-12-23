#include <cstdio>
#include <vector>
#include "Types.h"
#include "Geometry.h"
#include "ObjLoader.h"
#include "Tokenizer.h"
#include "Shader.h"
#include "Engine.h"
#include "Camera.h"
#include "Log.h"
#include "Framebuffer2D.h"
#include "Light.h"
#include "Spotlight.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define NUM_LIGHTS 1

void modifyCamera(Camera *cam);
// void createFBO();
// void destroyFBO();
void loadTexture(const char *filename, GLuint texID);
void setLightUniforms();

void drawScene();

GLuint fbo;
GLuint depthMap;
Geometry bunny;
Geometry plane;
std::vector<Light*> lights;

int main()
{
    fbo = 0;
    depthMap = 0;
    glen::Engine *engine = new glen::Engine();

    if(!engine->init(WINDOW_WIDTH,WINDOW_HEIGHT))
    {
        delete engine;
        return 0;
    }

    //float time;
    int timeLoc;
    int textureLoc;

    GLuint testTexture = 0;

    Camera cam;
    vec3 lookPos(0,0.3,0);

    //Framebuffer2D fboBack(WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    //fboBack.attachBuffer(FBO_DEPTH, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);

    Framebuffer2D fboFront(WINDOW_WIDTH, WINDOW_HEIGHT);
    fboFront.attachBuffer(FBO_DEPTH, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE);
    fboFront.attachBuffer(FBO_AUX0, GL_RG, GL_RG16F, GL_FLOAT);

    Spotlight spotlight;
    spotlight.setPosition(vec3(2,2.5,-2));
    spotlight.setLookAt(vec3(0,0.3,0));
    spotlight.setColor(vec3(1.0,0.9,0.7));

    cam.setPosition(0,10,10);
    cam.lookAt(&lookPos);

    mat4 modelMatrix(1.0), modelViewMatrix(1.0);
    mat4 textureMatrix;

    // createFBO();

    Shader linearDepthShader("resources/shaders/linearDepth_vert.glsl","resources/shaders/linearDepth_frag.glsl");
    Shader depthShader("resources/shaders/depth_vert.glsl","resources/shaders/depth_frag.glsl");
    Shader skinShader("resources/shaders/skin_vert.glsl", "resources/shaders/skin_frag.glsl");
    Shader basicShader("resources/shaders/basic_vert.glsl", "resources/shaders/basic_frag.glsl");
    Shader lightShader("resources/shaders/light_vert.glsl", "resources/shaders/light_frag.glsl");

    loadObj(bunny,"resources/meshes/head.obj",2.0f);
    bunny.translate(vec3(0,0.5,0));
    bunny.createStaticBuffers();

    Geometry fsquad;
    Geometry::sVertex v;

    v.position = vec3(-1,-1, 0); v.texCoord = vec2(0,0); fsquad.addVertex(v);
    v.position = vec3( 1,-1, 0); v.texCoord = vec2(1,0); fsquad.addVertex(v);
    v.position = vec3( 1, 1, 0); v.texCoord = vec2(1,1); fsquad.addVertex(v);
    v.position = vec3(-1, 1, 0); v.texCoord = vec2(0,1); fsquad.addVertex(v);

    fsquad.addTriangle(uvec3(0,1,2));
    fsquad.addTriangle(uvec3(0,2,3));

    fsquad.createStaticBuffers();

    #define PLANESIZE 10

    v.normal = vec3(0,1,0);
    v.position = vec3(-PLANESIZE,0,PLANESIZE);    v.texCoord = vec2(0,0); plane.addVertex(v);
    v.position = vec3(PLANESIZE,0,PLANESIZE);     v.texCoord = vec2(1,0); plane.addVertex(v);
    v.position = vec3(PLANESIZE,0,-PLANESIZE);    v.texCoord = vec2(1,1); plane.addVertex(v);
    v.position = vec3(-PLANESIZE,0,-PLANESIZE);   v.texCoord = vec2(0,1); plane.addVertex(v);

    plane.addTriangle(uvec3(0,1,2));
    plane.addTriangle(uvec3(0,2,3));

    plane.createStaticBuffers();

    timeLoc = skinShader.getUniformLocation("time");
    textureLoc = skinShader.getUniformLocation("texture0");

    glClearColor(1.0,1.0,1.0,1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);

    loadTexture("resources/textures/texture.tga", testTexture);
    //glBindTexture(GL_TEXTURE_2D, fboBack.getBufferHandle(FBO_DEPTH));
    // glBindTexture(GL_TEXTURE_2D, depthMap);

    while(true)
    {
        // LIGHT PASS

        depthShader.bind();

        for(int i=0; i<NUM_LIGHTS; i++)
        {
            spotlight.setup();
            spotlight.bindFbo();

            glClearDepth(1.0);
            glClear(GL_DEPTH_BUFFER_BIT);

            modelViewMatrix = spotlight.getViewMatrix() * modelMatrix;

            glUniformMatrix4fv(depthShader.getViewMatrixLocation(), 1, false, glm::value_ptr(modelViewMatrix));
            glUniformMatrix4fv(depthShader.getProjMatrixLocation(), 1, false, glm::value_ptr(spotlight.getProjMatrix()));

            drawScene();

            spotlight.unbindFbo();
        }

        depthShader.unbind();

        // END LIGHT PASS

        modifyCamera(&cam);
        cam.setup();
        modelViewMatrix = cam.getViewMatrix() * modelMatrix;
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        // BACK PASS

        // END BACK PASS

        glBindTexture(GL_TEXTURE_2D, spotlight.getShadowMap());
        // FRONT PASS

        glClearDepth(1.0);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glCullFace(GL_BACK);
        glDepthFunc(GL_LESS);

        lightShader.bind();

        textureLoc = lightShader.getUniformLocation("texture0");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 0);

        int textureMatrixLoc = lightShader.getUniformLocation("textureMatrix");
        if(textureMatrixLoc > -1)
        {
            textureMatrix = spotlight.getTextureMatrix() * modelMatrix;
            glUniformMatrix4fv(textureMatrixLoc, 1, false, glm::value_ptr(textureMatrix));
        }

        spotlight.setPositionUniform("spotlightPos");
        spotlight.setDirectionUniform("spotlightDir");
        spotlight.setColorUniform("spotlightColor");

        glUniformMatrix4fv(lightShader.getModelMatrixLocation(), 1, false, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(lightShader.getViewMatrixLocation(), 1, false, glm::value_ptr(cam.getViewMatrix()));
        glUniformMatrix4fv(lightShader.getProjMatrixLocation(), 1, false, glm::value_ptr(cam.getProjMatrix()));

        //bunny.draw();
        drawScene();

        lightShader.unbind();

        // END FRONT PASS

        //glBindTexture(GL_TEXTURE_2D, fboBack.getBufferHandle(FBO_DEPTH));
        //glBindTexture(GL_TEXTURE_2D, testTexture);

        // Draw to screen
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //basicShader.bind();

        // textureLoc = basicShader.getUniformLocation("texture0");
        // if(textureLoc>-1)
        //     glUniform1i(textureLoc, 0);

        // int lightMatrixLoc = lightShader.getUniformLocation("lightToViewMatrix");

        // if(lightMatrixLoc>-1)
        //     glUniformMatrix4fv(lightMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightToViewMatrix));

        // glUniformMatrix4fv(lightShader.getViewMatrixLocation(), 1, false, glm::value_ptr(modelViewMatrix));
        // glUniformMatrix4fv(lightShader.getProjMatrixLocation(), 1, false, glm::value_ptr(cam.getProjMatrix()));

        // glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        //fsquad.draw();
        // //bunny.draw();

        // basicShader.unbind();

        engine->swapBuffers();
        if(!glfwGetWindowParam(GLFW_OPENED) || glfwGetKey(GLFW_KEY_ESC) || glfwGetKey('Q'))
			break;
    }

    //clean up
    delete engine;

    // destroyFBO();

    return 0;
}

void modifyCamera(Camera *cam)
{
    static int oldMouseX=320, oldMouseY=240;
    static vec2 rotAngle;
    int mouseX, mouseY;

    glfwGetMousePos(&mouseX, &mouseY);
    vec2 mouseMove((float)(mouseX-oldMouseX), (float)(mouseY-oldMouseY));

    oldMouseX = mouseX; oldMouseY = mouseY;

    if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT))
        rotAngle += mouseMove * 0.01f;

    rotAngle.y = glm::clamp(rotAngle.y, -0.2f, 3.1415926535897932f * 0.5f);

    float dist = 1;

    cam->setPosition(dist*glm::cos(rotAngle.x), dist*glm::sin(rotAngle.y), dist*glm::sin(rotAngle.x));

    //mat = glm::rotate(mat4(), rotAngle.y, vec3(1,0,0));
    //mat = glm::rotate(mat, rotAngle.x, vec3(0,1,0));

}

void drawScene()
{
    bunny.draw();
    //plane.draw();
}

/*
 * loadTexture - load 8-bit texture data from a TGA file
 * and set up the corresponding texture object.     //STEGU
 */
void loadTexture(const char *filename, GLuint texID) {
  
    GLFWimage img; // Use intermediate GLFWimage to get width and height

    if(!glfwReadImage(filename, &img, GLFW_NO_RESCALE_BIT))
        logError("texture could not be loaded");

    glBindTexture( GL_TEXTURE_2D, texID );

    glfwLoadTextureImage2D( &img, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glfwFreeImage(&img); // Clean up the malloc()'ed data pointer

    glBindTexture( GL_TEXTURE_2D, 0 );
}

void setLightUniforms()
{

}