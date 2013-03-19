#ifndef SHADER_H
#define SHADER_H

/*#ifdef __APPLE__
    #define GLFW_INCLUDE_GL3
    #define GLFW_NO_GLU    
#else
    #include <GL/glew.h>
#endif

#include <GL/glfw.h>*/

#include "Types.h"

class Shader
{
private:
    GLuint program;
    GLuint vertexShader, fragmentShader;

    /*
     * Holders for predefined locations in the shader.
     * The predefined variables are:
     *
     * in_position: position data sent to vertex shader.
     * in_normal: normal data sent to vertex shader.
     * in_texCoord: texture coordinate data sent to vertex shader.
     *
     * viewMatrix: actually the model to view matrix
     * projMatrix: the projection matrix
     */
     
    GLint positionLoc, normalLoc, tangentLoc, texCoordLoc;
    GLint modelMatrixLoc, viewMatrixLoc, projMatrixLoc;

    char *vertexFile, *fragmentFile;
    bool compiled;

    static Shader *boundShader;
    
public:
    Shader();
    Shader(const char *vertFile, const char *fragFile);
    ~Shader();

    static Shader *getBoundShader() { return boundShader; }

    bool loadAndCompile();

    void setVertexFile(const char *vertFile);
    void setFragmentFile(const char *fragFile);

    const char *getVertexFile() { return vertexFile; }
    const char *getFragmentFile() { return fragmentFile; }

    GLint getAttributeLocation(const char *att);
    GLint getUniformLocation(const char *uni);

    GLint getPositionLocation() { return positionLoc; }
    GLint getNormalLocation() { return normalLoc; }
    GLint getTangentLocation() { return tangentLoc; }
    GLint getTexCoordLocation() { return texCoordLoc; }

    GLint getModelMatrixLocation() { return modelMatrixLoc; }
    GLint getViewMatrixLocation() { return viewMatrixLoc; }
    GLint getProjMatrixLocation() { return projMatrixLoc; }

    bool isCompiled() { return compiled; }

    void bind();
    void unbind();
};

#endif
