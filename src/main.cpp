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
#include "Spotlight.h"

#define TEXTURE_SPACE_WIDTH 1024
#define TEXTURE_SPACE_HEIGHT 1024

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define NUM_HDR_BLUR_PASSES 2

#define EXPOSURE_MIN 0.1
#define EXPOSURE_MAX 10.0
#define EXPOSURE_INC 0.1

#define BLOOM_MIN 0.1
#define BLOOM_MAX 1.0
#define BLOOM_INC 0.1

#define DENSITY_MIN 0.5
#define DENSITY_MAX 3.0
#define DENSITY_INC 0.1

#define RAIN_MIN 0.0
#define RAIN_MAX 1.0
#define RAIN_INC 0.1

#define ROTATION_SPEED 0.1

void modifyModel(mat4 &m);
void modifyCamera(Camera *cam);
void loadTexture(const char *filename, GLuint texID, int flags=0);

void drawScene();
void calcFPS();
void GLFWCALL keyCallback(int key, int action);

Geometry head;
Geometry plane;
std::vector<Spotlight*> lights;

float g_exposure;
float g_bloom;
float g_density;
float g_rain;

bool g_showSpec;

int main()
{
    glen::Engine *engine = new glen::Engine();

    if(!engine->init(WINDOW_WIDTH,WINDOW_HEIGHT))
    {
        delete engine;
        return 0;
    }

    glfwSetKeyCallback(keyCallback);

    g_exposure = 2.2;
    g_bloom = 0.3;
    g_density = 1.2;
    g_rain = 0.5;
    g_showSpec = false;

    int timeLoc;
    int textureMatrixLoc;
    int uniformLoc;
    //int textureLoc;

    GLuint testTexture = 0;
    GLuint colorMap = 0;
    GLuint normalMap = 0;
    GLuint specularMap = 0;

    glGenTextures(1, &testTexture);
    glGenTextures(1, &colorMap);
    glGenTextures(1, &normalMap);
    glGenTextures(1, &specularMap);

    Framebuffer2D fboFront(WINDOW_WIDTH, WINDOW_HEIGHT);
    fboFront.attachBuffer(FBO_DEPTH, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT);   // Front-Depth
    fboFront.attachBuffer(FBO_AUX0, GL_RGBA8, GL_RGBA, GL_FLOAT);                           // Front-Albedo + Subsurface structure
    fboFront.attachBuffer(FBO_AUX1, GL_RGB16F, GL_RGB, GL_FLOAT);                           // Front-XY-Normal + PackedSpecular (exponent + base)
    fboFront.attachBuffer(FBO_AUX2, GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_TRUE);                                       // Blurred version of final

    Framebuffer2D fboTexture(TEXTURE_SPACE_WIDTH, TEXTURE_SPACE_HEIGHT);
    fboTexture.attachBuffer(FBO_DEPTH, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
    fboTexture.attachBuffer(FBO_AUX0, GL_RGBA16F, GL_RGBA16F, GL_FLOAT);

    Framebuffer2D fboFinal(WINDOW_WIDTH, WINDOW_HEIGHT);
    fboFinal.attachBuffer(FBO_AUX0, GL_RGB16F, GL_RGB, GL_FLOAT);                           // Final radiance

    lights.push_back(new Spotlight());
    lights[0]->setPosition(vec3(0.1,0.4,-1.5));
    lights[0]->setLookAt(vec3(0,0.3,0));
    lights[0]->setColor(vec3(1.0,1.0,1.0));
    lights[0]->setLumen(5);
    lights[0]->setOuterAngle(35.0);

    // lights.push_back(new Spotlight());
    // lights[1]->setPosition(vec3(0.2,0.4,-1.5));
    // lights[1]->setLookAt(vec3(0,0.3,0));
    // lights[1]->setColor(vec3(1.0,1.0,1.0));
    // lights[1]->setLumen(4);
    // lights[1]->setOuterAngle(35.0);

    lights.push_back(new Spotlight());
    lights[1]->setPosition(vec3(0.0,0.4,1.5));
    lights[1]->setLookAt(vec3(0,0.3,0));
    lights[1]->setColor(vec3(1.0,1.0,1.0));
    lights[1]->setLumen(6);
    lights[1]->setOuterAngle(35.0);

    Camera cam;
    vec3 lookPos(0,0.3,0);
    cam.setPosition(0,10,10);
    cam.lookAt(&lookPos);

    mat4 modelMatrix(1.0), modelViewMatrix(1.0);
    mat4 inverseModelMatrix(1.0), inverseModelViewMatrix(1.0);
    mat4 textureMatrix;

    Shader depthShader("resources/shaders/depth_vert.glsl", "resources/shaders/depth_frag.glsl");
    Shader lightShader("resources/shaders/light_vert.glsl", "resources/shaders/light_frag.glsl");
    Shader frontShader("resources/shaders/front_vert.glsl", "resources/shaders/front_frag.glsl");
    Shader vBlurShader("resources/shaders/basic_vert.glsl", "resources/shaders/v_blur_frag.glsl");
    Shader hBlurShader("resources/shaders/basic_vert.glsl", "resources/shaders/h_blur_frag.glsl");
    Shader tonemapShader("resources/shaders/basic_vert.glsl", "resources/shaders/tonemap_frag.glsl");
    Shader specularShader("resources/shaders/basic_vert.glsl", "resources/shaders/specular_frag.glsl");
    Shader fsShader("resources/shaders/test_vert.glsl", "resources/shaders/test_frag.glsl");
    Shader tsShader("resources/shaders/texture_space_vert.glsl", "resources/shaders/texture_space_frag.glsl");

    vBlurShader.bind();

    uniformLoc = vBlurShader.getUniformLocation("width");
    if(uniformLoc > -1)
        glUniform1i(uniformLoc, WINDOW_WIDTH);

    uniformLoc = vBlurShader.getUniformLocation("height");
    if(uniformLoc > -1)
        glUniform1i(uniformLoc, WINDOW_HEIGHT);

    vBlurShader.unbind();

    hBlurShader.bind();

    uniformLoc = hBlurShader.getUniformLocation("width");
    if(uniformLoc > -1)
        glUniform1i(uniformLoc, WINDOW_WIDTH);

    uniformLoc = hBlurShader.getUniformLocation("height");
    if(uniformLoc > -1)
        glUniform1i(uniformLoc, WINDOW_HEIGHT);

    hBlurShader.unbind();

    loadObj(head,"resources/meshes/head.obj", 2.0f);
    head.translate(vec3(0,0.3,0));
    head.createStaticBuffers();

    Geometry fsquad;
    Geometry::sVertex v;

    v.position = vec3(-1,-1, 0); v.texCoord = vec2(0,0); fsquad.addVertex(v);
    v.position = vec3( 1,-1, 0); v.texCoord = vec2(1,0); fsquad.addVertex(v);
    v.position = vec3( 1, 1, 0); v.texCoord = vec2(1,1); fsquad.addVertex(v);
    v.position = vec3(-1, 1, 0); v.texCoord = vec2(0,1); fsquad.addVertex(v);

    fsquad.addTriangle(uvec3(0,1,2));
    fsquad.addTriangle(uvec3(0,2,3));

    fsquad.createStaticBuffers();

    loadTexture("resources/textures/texture.tga", testTexture);
    loadTexture("resources/textures/head.tga", colorMap);
    loadTexture("resources/textures/head_n.tga", normalMap);
    loadTexture("resources/textures/head_d.tga", specularMap); 

    glClearColor(0.0,0.0,0.0,0.0);
    glClearDepth(1.0);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);

    while(true)
    {
        calcFPS();
        modifyModel(modelMatrix);
        modifyCamera(&cam);

        cam.setup();
        modelViewMatrix = cam.getViewMatrix() * modelMatrix;
        inverseModelMatrix = glm::inverse(modelMatrix);
        inverseModelViewMatrix = inverseModelMatrix * cam.getInverseViewMatrix();

        glEnable(GL_DEPTH_TEST);
        glDepthMask(1);
        glColorMask(0,0,0,0);

        depthShader.bind();

        // LIGHT DEPTH PASS

        for(size_t i=0; i<lights.size(); ++i)
        {
            lights[i]->setup();
            lights[i]->bindFbo();

            glClear(GL_DEPTH_BUFFER_BIT);

            modelViewMatrix = lights[i]->getViewMatrix() * modelMatrix;

            glUniformMatrix4fv(depthShader.getViewMatrixLocation(), 1, false, glm::value_ptr(modelViewMatrix));
            glUniformMatrix4fv(depthShader.getProjMatrixLocation(), 1, false, glm::value_ptr(lights[i]->getProjMatrix()));

            drawScene();
        }

        // END LIGHT DEPTH PASS

        //glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        fboTexture.bind();

        tsShader.bind();

        glUniformMatrix4fv(tsShader.getViewMatrixLocation(), 1, false, glm::value_ptr(modelViewMatrix));

        head.draw();

        // No more depth comparisions that needs to be done, just full screen quads rendered.
        glDisable(GL_DEPTH_TEST);
        glDepthMask(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        fsShader.bind();
        glBindTexture(GL_TEXTURE_2D, fboTexture.getBufferHandle(FBO_AUX0));
        fsquad.draw();

        engine->swapBuffers();
        if(!glfwGetWindowParam(GLFW_OPENED) || glfwGetKey(GLFW_KEY_ESC))
			break;
    }

    for(size_t i=0; i<lights.size(); i++)
    {
        delete lights[i];                        
    }

    glDeleteTextures(1, &testTexture);
    glDeleteTextures(1, &colorMap);
    glDeleteTextures(1, &normalMap);
    glDeleteTextures(1, &specularMap);

    //clean up
    delete engine;

    return 0;
}

void modifyCamera(Camera *cam)
{
    static int oldMouseX=320, oldMouseY=240, oldMouseZ=1;
    static vec2 rotAngle = vec2(PI,0.5*PI);
    static float dist;
    int mouseX, mouseY, mouseZ;

    glfwGetMousePos(&mouseX, &mouseY);
    mouseZ = glfwGetMouseWheel();

    vec2 mouseMove((float)(mouseX-oldMouseX), (float)(mouseY-oldMouseY));
    dist += (float)(mouseZ-oldMouseZ) * 0.1f;

    oldMouseX = mouseX; oldMouseY = mouseY; oldMouseZ = mouseZ;

    if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT))
        rotAngle += mouseMove * 0.02f;

    rotAngle.y = glm::clamp(rotAngle.y, 0.02f, PI * 0.98f);
    dist = glm::clamp(dist, 0.6f, 10.0f);

    float theta = -rotAngle.y;
    float phi = -rotAngle.x;

    cam->setPosition(dist*glm::sin(theta)*glm::sin(phi), dist*glm::cos(theta), dist*glm::sin(theta)*glm::cos(phi));
}

void drawScene()
{
    head.draw();
}

/*
 * loadTexture - load 8-bit texture data from a TGA file
 * and set up the corresponding texture object.     //STEGU
 */
void loadTexture(const char *filename, GLuint texID, int flags) {
  
    GLFWimage img; // Use intermediate GLFWimage to get width and height

    logNote("Attempting to load texture %s", filename);

    if(!glfwReadImage(filename, &img, GLFW_NO_RESCALE_BIT))
    {
        logError("texture-image could not be read");
        return;
    }

    glBindTexture( GL_TEXTURE_2D, texID );

    if(!glfwLoadTextureImage2D( &img, flags ))
    {
        logError("texture could not be loaded");
        glBindTexture( GL_TEXTURE_2D, 0 );
        return;
    }

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glfwFreeImage(&img); // Clean up the malloc()'ed data pointer

    glBindTexture( GL_TEXTURE_2D, 0 );
}

void GLFWCALL keyCallback(int key, int action)
{
    if(key == 'W' && action == GLFW_PRESS)
    {
        g_exposure = glm::clamp(g_exposure + EXPOSURE_INC, EXPOSURE_MIN, EXPOSURE_MAX);
    }
    else if(key == 'S' && action == GLFW_PRESS)
    {
        g_exposure = glm::clamp(g_exposure - EXPOSURE_INC, EXPOSURE_MIN, EXPOSURE_MAX);
    }
    else if (key == 'E' && action == GLFW_PRESS)
    {
        g_bloom = glm::clamp(g_bloom + BLOOM_INC, BLOOM_MIN, BLOOM_MAX);
    }
    else if(key == 'D' && action == GLFW_PRESS)
    {
        g_bloom = glm::clamp(g_bloom - BLOOM_INC, BLOOM_MIN, BLOOM_MAX);
    }
    else if (key == 'R' && action == GLFW_PRESS)
    {
        g_density = glm::clamp(g_density + DENSITY_INC, DENSITY_MIN, DENSITY_MAX);
    }
    else if(key == 'F' && action == GLFW_PRESS)
    {
        g_density = glm::clamp(g_density - DENSITY_INC, DENSITY_MIN, DENSITY_MAX);
    }
    else if (key == 'T' && action == GLFW_PRESS)
    {
        g_rain = glm::clamp(g_rain + RAIN_INC, RAIN_MIN, RAIN_MAX);
    }
    else if(key == 'G' && action == GLFW_PRESS)
    {
        g_rain = glm::clamp(g_rain - RAIN_INC, RAIN_MIN, RAIN_MAX);
    }
    else if(key == '1' && action == GLFW_PRESS)
    {
        g_showSpec = !g_showSpec;
    }
}

void calcFPS()
{
    static double t0=0.0;
    static int frameCount=0;
    static char title[256];

    double t = glfwGetTime();

    if( (t - t0) > 0.25 )
    {
        double fps = (double)frameCount / (t - t0);
        frameCount = 0;
        t0 = t;

        sprintf(title, "Fake-SSS FPS: %3.1f, Exposure(W/S): %2.1f, Bloom(E/D) %1.1f, Density(R/F) %1.2f, Rain(T/G) %1.1f", fps, g_exposure, g_bloom, g_density, g_rain);
        glfwSetWindowTitle(title);
    }
    frameCount++;
}

void modifyModel( mat4 &m )
{
    static float angle = 0.0;
    angle += ROTATION_SPEED;

    m = glm::rotate(mat4(), angle, vec3(0.0,1.0,0.0));
}