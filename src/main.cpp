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

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define NUM_HDR_BLUR_PASSES 1

#define EXPOSURE_MIN 0.1
#define EXPOSURE_MAX 10.0
#define EXPOSURE_INC 0.1

#define BLOOM_MIN 0.1
#define BLOOM_MAX 1.0
#define BLOOM_INC 0.1

#define DENSITY_MIN 0.5
#define DENSITY_MAX 3.0
#define DENSITY_INC 0.1

#define ROTATION_SPEED 0.1

void modifyModel(mat4 &m);
void modifyCamera(Camera *cam);
void loadTexture(const char *filename, GLuint texID);

void drawScene();
void calcFPS();
void GLFWCALL keyCallback(int key, int action);

GLuint fbo;
GLuint depthMap;
Geometry head;
Geometry plane;
std::vector<Spotlight*> lights;

float g_exposure;
float g_bloom;
float g_density;

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

    glfwSetKeyCallback(keyCallback);

    g_exposure = 2.2;
    g_bloom = 0.3;
    g_density = 1.0;

    int timeLoc;
    int textureLoc;

    GLuint testTexture = 0;
    GLuint colorMap = 0;
    GLuint normalMap = 0;

    glGenTextures(1, &testTexture);
    glGenTextures(1, &colorMap);
    glGenTextures(1, &normalMap);

    Framebuffer2D fboFront(WINDOW_WIDTH, WINDOW_HEIGHT);
    fboFront.attachBuffer(FBO_DEPTH, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT);   // Front-Depth
    fboFront.attachBuffer(FBO_AUX0, GL_RGBA8, GL_RGBA, GL_FLOAT);                           // Front-Albedo + Subsurface structure
    fboFront.attachBuffer(FBO_AUX1, GL_RGBA16F, GL_RGBA, GL_FLOAT);                         // Front-XY-Normal-And-TexCoords

    Framebuffer2D fboLight(WINDOW_WIDTH, WINDOW_HEIGHT);
    fboLight.attachBuffer(FBO_AUX0, GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR, GL_LINEAR);     // Surface radiance
    fboLight.attachBuffer(FBO_AUX1, GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR, GL_LINEAR);     // Subsurface radiance
    fboLight.attachBuffer(FBO_AUX2, GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR, GL_LINEAR);     // Specular radiance

    Framebuffer2D fboBlur(WINDOW_WIDTH, WINDOW_HEIGHT);
    fboBlur.attachBuffer(FBO_AUX0, GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_TRUE);                                       // Blured version of final

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
    lights[1]->setColor(vec3(1.0,0.8,0.8));
    lights[1]->setLumen(5);
    lights[1]->setOuterAngle(35.0);

    Camera cam;
    vec3 lookPos(0,0.3,0);
    cam.setPosition(0,10,10);
    cam.lookAt(&lookPos);

    mat4 modelMatrix(1.0), modelViewMatrix(1.0);
    mat4 inverseModelMatrix(1.0), inverseModelViewMatrix(1.0);
    mat4 textureMatrix;

    Shader linearDepthShader("resources/shaders/linearDepth_vert.glsl", "resources/shaders/linearDepth_frag.glsl");
    Shader depthShader("resources/shaders/depth_vert.glsl", "resources/shaders/depth_frag.glsl");
    Shader skinShader("resources/shaders/skin_vert.glsl", "resources/shaders/skin_frag.glsl");
    Shader basicShader("resources/shaders/basic_vert.glsl", "resources/shaders/basic_frag.glsl");
    Shader lightShader("resources/shaders/light_vert.glsl", "resources/shaders/light_frag.glsl");
    Shader backShader("resources/shaders/back_vert.glsl", "resources/shaders/back_frag.glsl");
    Shader frontShader("resources/shaders/front_vert.glsl", "resources/shaders/front_frag.glsl");
    Shader vBlurShader("resources/shaders/basic_vert.glsl", "resources/shaders/v_blur_frag.glsl");
    Shader hBlurShader("resources/shaders/basic_vert.glsl", "resources/shaders/h_blur_frag.glsl");
    Shader compositShader("resources/shaders/basic_vert.glsl", "resources/shaders/composit_frag.glsl");
    Shader tonemapShader("resources/shaders/basic_vert.glsl", "resources/shaders/tonemap_frag.glsl");

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

    timeLoc = skinShader.getUniformLocation("time");
    textureLoc = skinShader.getUniformLocation("texture0");
    int textureMatrixLoc;
    int uniformLoc;

    glClearColor(0.0,0.0,0.0,0.0);
    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);

    while(true)
    {
        calcFPS();
        modifyModel(modelMatrix);
        modifyCamera(&cam);

        cam.setup();
        modelViewMatrix = cam.getViewMatrix() * modelMatrix;
        inverseModelMatrix = glm::inverse(modelMatrix);
        inverseModelViewMatrix = inverseModelMatrix * cam.getInverseViewMatrix();

        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(1);
        glColorMask(0,0,0,0);
        glClearDepth(1.0);
        glCullFace(GL_BACK);
        glDepthFunc(GL_LESS);

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

            lights[i]->unbindFbo();
        }

        depthShader.unbind();

        // END LIGHT DEPTH PASS

        glColorMask(1,1,1,1);

        // FRONT PASS

        fboFront.bind();

        frontShader.bind();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);

        glCullFace(GL_BACK);
        glDepthFunc(GL_LESS);

        glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        textureLoc = frontShader.getUniformLocation("texture0");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 0);

        textureLoc = frontShader.getUniformLocation("texture1");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 1);

        glUniformMatrix4fv(frontShader.getModelMatrixLocation(), 1, false, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(frontShader.getViewMatrixLocation(), 1, false, glm::value_ptr(cam.getViewMatrix()));
        glUniformMatrix4fv(frontShader.getProjMatrixLocation(), 1, false, glm::value_ptr(cam.getProjMatrix()));

        uniformLoc = frontShader.getUniformLocation("invModelMatrix");
        if(uniformLoc > -1)
            glUniformMatrix4fv(uniformLoc, 1, false, glm::value_ptr(inverseModelMatrix));

        uniformLoc = frontShader.getUniformLocation("camPos");
        if(uniformLoc > -1)
            glUniform3fv(uniformLoc, 1, glm::value_ptr(cam.getPosition()));

        drawScene();

        frontShader.unbind();

        fboFront.unbind();

        // END FRONT PASS

        //glStencilFunc( GL_NOTEQUAL, 1, 1 );
        //glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

        // No more depth comparisions that needs to be done, just full screen quads rendered.
        glDisable(GL_DEPTH_TEST);
        glDepthMask(0);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        //Blend each lights contribution into the buffer
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        // LIGHT PASS

        fboLight.bind();

        glClear(GL_COLOR_BUFFER_BIT);

        lightShader.bind();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fboFront.getBufferHandle(FBO_DEPTH));

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fboFront.getBufferHandle(FBO_AUX1));

        textureMatrixLoc = lightShader.getUniformLocation("textureMatrix");

        textureLoc = lightShader.getUniformLocation("texture0");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 0);

        textureLoc = lightShader.getUniformLocation("texture1");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 1);

        textureLoc = lightShader.getUniformLocation("texture2");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 2);

        glUniformMatrix4fv(lightShader.getViewMatrixLocation(), 1, false, glm::value_ptr(cam.getViewMatrix()));

        uniformLoc = lightShader.getUniformLocation("invViewMatrix");
        if(uniformLoc > -1)
            glUniformMatrix4fv(uniformLoc, 1, false, glm::value_ptr(cam.getInverseViewMatrix()));

        uniformLoc = lightShader.getUniformLocation("density");
        if(uniformLoc > -1)
            glUniform1f(uniformLoc, g_density);

        glActiveTexture(GL_TEXTURE2);

        for(size_t i=0; i<lights.size(); ++i)
        {
            glBindTexture(GL_TEXTURE_2D, lights[i]->getShadowMap());
            
            if(textureMatrixLoc > -1)
            {
                textureMatrix = lights[i]->getTextureMatrix() * cam.getInverseViewMatrix();
                glUniformMatrix4fv(textureMatrixLoc, 1, false, glm::value_ptr(textureMatrix));
            }

            lights[i]->setPositionUniform("spotlightPos");
            lights[i]->setDirectionUniform("spotlightDir");
            lights[i]->setColorUniform("spotlightColor");

            fsquad.draw();
        }

        lightShader.unbind();
        fboLight.unbind();

        glDisable(GL_BLEND);

        // END LIGHT PASS

        // COMPOSIT PASS

        // use as temp
        fboFinal.bind();

        glClear(GL_COLOR_BUFFER_BIT);

        compositShader.bind();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fboFront.getBufferHandle(FBO_DEPTH));

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fboFront.getBufferHandle(FBO_AUX0));

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, fboLight.getBufferHandle(FBO_AUX0));

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, fboLight.getBufferHandle(FBO_AUX1));

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, fboFront.getBufferHandle(FBO_AUX1));


        textureLoc = compositShader.getUniformLocation("texture0");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 0);

        textureLoc = compositShader.getUniformLocation("texture1");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 1);

        textureLoc = compositShader.getUniformLocation("texture2");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 2);

        textureLoc = compositShader.getUniformLocation("texture3");
        if(textureLoc > -1)
            glUniform1i(textureLoc, 3);

        uniformLoc = compositShader.getUniformLocation("invModelViewMatrix");
        if(uniformLoc > -1)
            glUniformMatrix4fv(uniformLoc, 1, false, glm::value_ptr(inverseModelViewMatrix));

        uniformLoc = compositShader.getUniformLocation("camPos");
        if(uniformLoc > -1)
            glUniform3fv(uniformLoc, 1, glm::value_ptr(cam.getPosition()));

        fsquad.draw();

        compositShader.unbind();

        fboFinal.unbind();

        // END COMPOSIT PASS

        // HDR-BLUR PASS

        glActiveTexture(GL_TEXTURE0);

        for(int i=0; i<NUM_HDR_BLUR_PASSES; i++)
        {
            // use as tempbuffer
            fboLight.bind();
            glClear(GL_COLOR_BUFFER_BIT);
            hBlurShader.bind();

            if(i == 0)
                glBindTexture(GL_TEXTURE_2D, fboFinal.getBufferHandle(FBO_AUX0));
            else
                glBindTexture(GL_TEXTURE_2D, fboBlur.getBufferHandle(FBO_AUX0));

            textureLoc = hBlurShader.getUniformLocation("texture0");
            if(textureLoc > -1)
                glUniform1i(textureLoc, 0);

            uniformLoc = hBlurShader.getUniformLocation("width");
            if(uniformLoc > -1)
                glUniform1i(uniformLoc, WINDOW_WIDTH);

            uniformLoc = hBlurShader.getUniformLocation("height");
            if(uniformLoc > -1)
                glUniform1i(uniformLoc, WINDOW_HEIGHT);

            // draw!
            fsquad.draw();

            hBlurShader.unbind();
            fboLight.unbind();

            fboBlur.bind();
            vBlurShader.bind();

            glBindTexture(GL_TEXTURE_2D, fboLight.getBufferHandle(FBO_AUX0));

            textureLoc = vBlurShader.getUniformLocation("texture0");
            if(textureLoc > -1)
                glUniform1i(textureLoc, 0);

            uniformLoc = vBlurShader.getUniformLocation("width");
            if(uniformLoc > -1)
                glUniform1i(uniformLoc, WINDOW_WIDTH);

            uniformLoc = vBlurShader.getUniformLocation("height");
            if(uniformLoc > -1)
                glUniform1i(uniformLoc, WINDOW_HEIGHT);

            // draw!
            fsquad.draw();

            vBlurShader.unbind();
            fboBlur.unbind();
        }

        // END HDR-BLUR PASS

        // DRAW TO SCREEN
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);

        glCullFace(GL_BACK);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fboFinal.getBufferHandle(FBO_AUX0));

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fboBlur.getBufferHandle(FBO_AUX0));
        glGenerateMipmap(GL_TEXTURE_2D);

        tonemapShader.bind();

        textureLoc = tonemapShader.getUniformLocation("texture0");
        if(textureLoc>-1)
            glUniform1i(textureLoc, 0);

        textureLoc = tonemapShader.getUniformLocation("texture1");
        if(textureLoc>-1)
            glUniform1i(textureLoc, 1);

        uniformLoc = tonemapShader.getUniformLocation("exposure");
        if(uniformLoc>-1)
            glUniform1f(uniformLoc, g_exposure);

        uniformLoc = tonemapShader.getUniformLocation("bloom");
        if(uniformLoc>-1)
            glUniform1f(uniformLoc, g_bloom);

        fsquad.draw();

        tonemapShader.unbind();

        engine->swapBuffers();
        if(!glfwGetWindowParam(GLFW_OPENED) || glfwGetKey(GLFW_KEY_ESC))
			break;
    }

    for(size_t i=0; i<lights.size(); i++)
    {
        delete lights[i];
    }

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

        sprintf(title, "Fake-SSS FPS: %3.1f, Exposure(W/S): %2.1f, Bloom(E/D) %1.1f, Density(R/F) %1.2f", fps, g_exposure, g_bloom, g_density);
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