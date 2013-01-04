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
Geometry head;
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
    int textureSizeLoc;

    GLuint testTexture = 0;
    GLuint colorMap = 0;
    GLuint normalMap = 0;

    glGenTextures(1, &testTexture);
    glGenTextures(1, &colorMap);
    glGenTextures(1, &normalMap);

    Camera cam;
    vec3 lookPos(0,0.3,0);

    Framebuffer2D fboBack(WINDOW_WIDTH, WINDOW_HEIGHT);
    fboBack.attachBuffer(FBO_DEPTH, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);  // Back-Depth
    fboBack.attachBuffer(FBO_AUX0, GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR, GL_LINEAR);                        // Back-Light

    Framebuffer2D fboFront(WINDOW_WIDTH, WINDOW_HEIGHT);
    fboFront.attachBuffer(FBO_DEPTH, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT); // Front-Depth
    fboFront.attachBuffer(FBO_AUX0, GL_RGB8, GL_RGB, GL_FLOAT);                         // Front-Albedo
    fboFront.attachBuffer(FBO_AUX1, GL_RG16F, GL_RG, GL_FLOAT);                         // Front-XY-Normal
    fboFront.attachBuffer(FBO_AUX2, GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR, GL_LINEAR);                       // Front-Light

    Framebuffer2D fboBackBlur(WINDOW_WIDTH, WINDOW_HEIGHT);
    fboBackBlur.attachBuffer(FBO_AUX0, GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR, GL_LINEAR);                    // Back-Light-Blured

    Framebuffer2D fboFrontBlur(WINDOW_WIDTH, WINDOW_HEIGHT);
    fboFrontBlur.attachBuffer(FBO_AUX0, GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR, GL_LINEAR);                        // Front-Light-Blured

    Framebuffer2D fboFinal(WINDOW_WIDTH, WINDOW_HEIGHT);
    fboFinal.attachBuffer(FBO_AUX0, GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR, GL_LINEAR);                        // Final intensities / temp

    Spotlight spotlight;
    spotlight.setPosition(vec3(2,2.5,-2));
    spotlight.setLookAt(vec3(0,0.3,0));
    spotlight.setColor(vec3(1.0,0.9,0.7));

    cam.setPosition(0,10,10);
    cam.lookAt(&lookPos);

    mat4 modelMatrix(1.0), modelViewMatrix(1.0);
    mat4 textureMatrix;

    Shader linearDepthShader("resources/shaders/linearDepth_vert.glsl","resources/shaders/linearDepth_frag.glsl");
    Shader depthShader("resources/shaders/depth_vert.glsl","resources/shaders/depth_frag.glsl");
    Shader skinShader("resources/shaders/skin_vert.glsl", "resources/shaders/skin_frag.glsl");
    Shader basicShader("resources/shaders/basic_vert.glsl", "resources/shaders/basic_frag.glsl");
    Shader lightShader("resources/shaders/light_vert.glsl", "resources/shaders/light_frag.glsl");
    Shader backShader("resources/shaders/back_vert.glsl", "resources/shaders/back_frag.glsl");
    Shader frontShader("resources/shaders/front_vert.glsl", "resources/shaders/front_frag.glsl");
    Shader vBlurShader("resources/shaders/basic_vert.glsl", "resources/shaders/v_blur_frag.glsl");
    Shader hBlurShader("resources/shaders/basic_vert.glsl", "resources/shaders/h_blur_frag.glsl");

    loadObj(head,"resources/meshes/head.obj",2.0f);
    head.translate(vec3(0,0.5,0));
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

    // #define PLANESIZE 10

    // v.normal = vec3(0,1,0);
    // v.position = vec3(-PLANESIZE,0,PLANESIZE);    v.texCoord = vec2(0,0); plane.addVertex(v);
    // v.position = vec3(PLANESIZE,0,PLANESIZE);     v.texCoord = vec2(1,0); plane.addVertex(v);
    // v.position = vec3(PLANESIZE,0,-PLANESIZE);    v.texCoord = vec2(1,1); plane.addVertex(v);
    // v.position = vec3(-PLANESIZE,0,-PLANESIZE);   v.texCoord = vec2(0,1); plane.addVertex(v);

    // plane.addTriangle(uvec3(0,1,2));
    // plane.addTriangle(uvec3(0,2,3));

    // plane.createStaticBuffers();

    loadTexture("resources/textures/texture.tga", testTexture);
    loadTexture("resources/textures/head.tga", colorMap);
    loadTexture("resources/textures/head_n.tga", normalMap);

    timeLoc = skinShader.getUniformLocation("time");
    textureLoc = skinShader.getUniformLocation("texture0");
    int textureMatrixLoc;

    glClearColor(1.0,1.0,1.0,1.0);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);

    //glBindTexture(GL_TEXTURE_2D, fboBack.getBufferHandle(FBO_DEPTH));
    //glBindTexture(GL_TEXTURE_2D, depthMap);

    while(true)
    {
        glClearDepth(1.0);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // LIGHT PASS

        depthShader.bind();

        spotlight.setup();
        spotlight.bindFbo();

        glClear(GL_DEPTH_BUFFER_BIT);

        modelViewMatrix = spotlight.getViewMatrix() * modelMatrix;

        glUniformMatrix4fv(depthShader.getViewMatrixLocation(), 1, false, glm::value_ptr(modelViewMatrix));
        glUniformMatrix4fv(depthShader.getProjMatrixLocation(), 1, false, glm::value_ptr(spotlight.getProjMatrix()));

        drawScene();

        spotlight.unbindFbo();

        depthShader.unbind();

        // END LIGHT PASS

        modifyCamera(&cam);
        cam.setup();
        modelViewMatrix = cam.getViewMatrix() * modelMatrix;
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, spotlight.getShadowMap());

        // BACK PASS

        glCullFace(GL_FRONT);
        glDepthFunc(GL_LESS);

        backShader.bind();

        fboBack.bind();

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        textureLoc = backShader.getUniformLocation("texture0");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 0);

        textureMatrixLoc = backShader.getUniformLocation("textureMatrix");
        if(textureMatrixLoc > -1)
        {
            textureMatrix = spotlight.getTextureMatrix() * modelMatrix;
            glUniformMatrix4fv(textureMatrixLoc, 1, false, glm::value_ptr(textureMatrix));
        }

        spotlight.setPositionUniform("spotlightPos");
        spotlight.setDirectionUniform("spotlightDir");
        spotlight.setColorUniform("spotlightColor");

        glUniformMatrix4fv(backShader.getModelMatrixLocation(), 1, false, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(backShader.getViewMatrixLocation(), 1, false, glm::value_ptr(cam.getViewMatrix()));
        glUniformMatrix4fv(backShader.getProjMatrixLocation(), 1, false, glm::value_ptr(cam.getProjMatrix()));

        drawScene();

        fboBack.unbind();

        backShader.unbind();

        // END BACK PASS

        // FRONT PASS

        frontShader.bind();

        fboFront.bind();

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, colorMap);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normalMap);

        glCullFace(GL_BACK);
        glDepthFunc(GL_LESS);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        textureLoc = frontShader.getUniformLocation("texture0");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 0);

        textureLoc = frontShader.getUniformLocation("texture1");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 1);

        textureLoc = frontShader.getUniformLocation("texture2");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 2);

        textureMatrixLoc = frontShader.getUniformLocation("textureMatrix");
        if(textureMatrixLoc > -1)
        {
            textureMatrix = spotlight.getTextureMatrix() * modelMatrix;
            glUniformMatrix4fv(textureMatrixLoc, 1, false, glm::value_ptr(textureMatrix));
        }

        spotlight.setPositionUniform("spotlightPos");
        spotlight.setDirectionUniform("spotlightDir");
        spotlight.setColorUniform("spotlightColor");

        glUniformMatrix4fv(frontShader.getModelMatrixLocation(), 1, false, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(frontShader.getViewMatrixLocation(), 1, false, glm::value_ptr(cam.getViewMatrix()));
        glUniformMatrix4fv(frontShader.getProjMatrixLocation(), 1, false, glm::value_ptr(cam.getProjMatrix()));

        drawScene();

        fboFront.unbind();

        frontShader.unbind();

        // END FRONT PASS

        // No more depth comparisions that needs to be done, just full screen quads rendered.
        glDisable(GL_DEPTH_TEST);

        glActiveTexture(GL_TEXTURE0);

        // BLUR PASS

        // BACK LIGHTING VERTICAL BLUR

        vBlurShader.bind();
        fboFinal.bind();
        glClear(GL_COLOR_BUFFER_BIT);

        textureLoc = vBlurShader.getUniformLocation("texture0");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 0);

        textureSizeLoc = vBlurShader.getUniformLocation("textureSize");
        if(textureSizeLoc > -1)
            glUniform1f(textureSizeLoc, WINDOW_HEIGHT);

        glBindTexture(GL_TEXTURE_2D, fboBack.getBufferHandle(FBO_AUX0));

        fsquad.draw();

        // BACK LIGHTING HORIZONTAL BLUR

        hBlurShader.bind();
        fboBackBlur.bind();
        glClear(GL_COLOR_BUFFER_BIT);

        textureLoc = hBlurShader.getUniformLocation("texture0");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 0);

        textureSizeLoc = hBlurShader.getUniformLocation("textureSize");
        if(textureSizeLoc > -1)
            glUniform1f(textureSizeLoc, WINDOW_WIDTH);

        glBindTexture(GL_TEXTURE_2D, fboFinal.getBufferHandle(FBO_AUX0));

        fsquad.draw();
        hBlurShader.unbind();

        // END BLUR PASS

        // DRAW TO SCREEN

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fboBackBlur.getBufferHandle(FBO_AUX0));
        //glBindTexture(GL_TEXTURE_2D, spotlight.getShadowMap());
        //glBindTexture(GL_TEXTURE_2D, testTexture);

        // // Draw to screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);

        glCullFace(GL_BACK);
        glClear(GL_COLOR_BUFFER_BIT);

        basicShader.bind();

        textureLoc = basicShader.getUniformLocation("texture0");
        if(textureLoc>-1)
            glUniform1i(textureLoc, 0);

        fsquad.draw();

        basicShader.unbind();

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
    static int oldMouseX=320, oldMouseY=240, oldMouseZ=1;
    static vec2 rotAngle;
    static float dist;
    int mouseX, mouseY, mouseZ;

    glfwGetMousePos(&mouseX, &mouseY);
    vec2 mouseMove((float)(mouseX-oldMouseX), (float)(mouseY-oldMouseY));

    oldMouseX = mouseX; oldMouseY = mouseY;

    if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT))
        rotAngle += mouseMove * 0.01f;

    rotAngle.y = glm::clamp(rotAngle.y, 0.0f, PI * 1.0f);

    mouseZ = glfwGetMouseWheel();

    dist += (float)(mouseZ-oldMouseZ) * 0.1f;

    dist = glm::clamp(dist, 0.1f, 10.0f);

    oldMouseZ = mouseZ;

    float theta = rotAngle.y;
    float phi = rotAngle.x;

    cam->setPosition(dist*glm::sin(theta)*glm::cos(phi), dist*glm::sin(theta)*glm::sin(phi), dist*glm::cos(theta));

    //mat = glm::rotate(mat4(), rotAngle.y, vec3(1,0,0));
    //mat = glm::rotate(mat, rotAngle.x, vec3(0,1,0));

}

void drawScene()
{
    head.draw();
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
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glfwFreeImage(&img); // Clean up the malloc()'ed data pointer

    glBindTexture( GL_TEXTURE_2D, 0 );
}

void setLightUniforms()
{

}