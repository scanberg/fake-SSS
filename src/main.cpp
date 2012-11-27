#include "Types.h"
#include "Geometry.h"
#include "ObjLoader.h"
#include "Tokenizer.h"
#include "Shader.h"
#include "Engine.h"
#include "Camera.h"
#include "Log.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

void setupModelMatrix(mat4 &mat);
void createFBO();
void destroyFBO();
void loadTexture(const char *filename, GLuint texID);

GLuint fbo;
GLuint depthMap;

int main()
{
    fbo = UNUSED_ADRESS;
    depthMap = UNUSED_ADRESS;
    glen::Engine *engine = new glen::Engine();

    if(!engine->init(WINDOW_WIDTH,WINDOW_HEIGHT))
    {
        delete engine;
        return 0;
    }

    float time;
    int timeLoc;
    int textureLoc;

    glen::Camera cam;
    vec3 lookPos(0,2,0);

    cam.setPosition(0,10,10);
    cam.lookAt(&lookPos);

    mat4 modelMatrix, modelViewMatrix;

    createFBO();

    glen::Shader depthShader("resources/shaders/depth_vert.glsl","resources/shaders/depth_frag.glsl");
    glen::Shader skinShader("resources/shaders/skin_vert.glsl", "resources/shaders/skin_frag.glsl");

    glen::Geometry bunny;
    glen::loadObj(bunny,"resources/meshes/bunny.obj",0.8f);
    bunny.createStaticBuffers();

    glen::Geometry fsquad;
    glen::Geometry::sVertex v;
    v.position = vec3(-1,-1,0);
    v.texCoord = vec2(0,0);
    fsquad.addVertex(v);

    v.position = vec3(1,-1,0);
    v.texCoord = vec2(1,0);
    fsquad.addVertex(v);

    v.position = vec3(1,1,0);
    v.texCoord = vec2(1,1);
    fsquad.addVertex(v);

    v.position = vec3(-1,1,0);
    v.texCoord = vec2(0,1);
    fsquad.addVertex(v);

    fsquad.addTriangle(uvec3(0,1,2));
    fsquad.addTriangle(uvec3(0,2,3));

    fsquad.createStaticBuffers();

    timeLoc = skinShader.getUniformLocation("time");
    textureLoc = skinShader.getUniformLocation("texture0");

    glClearColor(1.0,1.0,1.0,1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, depthMap);

    GLuint testTexture;

    loadTexture("resources/textures/texture.tga", testTexture);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    while(true)
    {
        cam.setup();

        setupModelMatrix(modelMatrix);
        modelViewMatrix = cam.getViewMatrix() * modelMatrix;

        depthShader.bind();

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearDepth(0.0);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glDepthFunc(GL_GREATER);

        glUniformMatrix4fv(depthShader.getViewMatrixLocation(), 1, false, glm::value_ptr(modelViewMatrix));
        glUniformMatrix4fv(depthShader.getProjMatrixLocation(), 1, false, glm::value_ptr(cam.getProjMatrix()));

        // first pass, write linear depth of the back-faces.
        bunny.draw();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearDepth(1.0);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glCullFace(GL_BACK);
        glDepthFunc(GL_LESS);

        skinShader.bind();

        if(timeLoc > -1)
        {
            time = glfwGetTime();
            glUniform1f(timeLoc, time);
        }

        if(textureLoc > -1)
        {
            glUniform1i(textureLoc, 0);
        }

        //glBindTexture(GL_TEXTURE_2D, depthMap);

        glUniformMatrix4fv(skinShader.getViewMatrixLocation(), 1, false, glm::value_ptr(modelViewMatrix));
        glUniformMatrix4fv(skinShader.getProjMatrixLocation(), 1, false, glm::value_ptr(cam.getProjMatrix()));

        bunny.draw();

        skinShader.unbind();

        if(glfwGetKey(GLFW_KEY_SPACE))
            skinShader.loadAndCompile();

        engine->swapBuffers();
        if(!glfwGetWindowParam(GLFW_OPENED) || glfwGetKey(GLFW_KEY_ESC) || glfwGetKey('Q'))
			break;
    }

    destroyFBO();

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

void createFBO()
{
    glGenTextures(1,&depthMap);

    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0,GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1,&fbo);
    
    //switch to our fbo so we can bind stuff to it
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, depthMap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // check FBO status
    GLenum FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(FBOstatus != GL_FRAMEBUFFER_COMPLETE)
        printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");
    
    // Go back to regular frame buffer rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void destroyFBO()
{
    if(fbo != UNUSED_ADRESS)
        glDeleteFramebuffers(1,&fbo);
    if(depthMap != UNUSED_ADRESS)
        glDeleteTextures(1,&depthMap);
}

/*
 * loadTexture - load 8-bit texture data from a TGA file
 * and set up the corresponding texture object.
 */
void loadTexture(const char *filename, GLuint texID) {
  
    GLFWimage img; // Use intermediate GLFWimage to get width and height

    if(!glfwReadImage(filename, &img, GLFW_NO_RESCALE_BIT))
    {
        logError("texture could not be loaded");
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, texID );

    glfwLoadTextureImage2D( &img, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glfwFreeImage(&img); // Clean up the malloc()'ed data pointer
}
