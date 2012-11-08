#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <map>

#ifdef WIN32
#include <GL/glew.h>
#endif
#include <GL/glfw.h>

namespace glen
{
    class Shader
    {
    protected:
        GLuint program;
        GLuint vertexShader, fragmentShader;

        bool compiled;

    public:
        Shader();
        Shader( const char *vertexFile, const char *fragmentFile );
        ~Shader();

        void init(const char *vertexFile, const char *fragmentFile);

        GLint getAttributeLocation(const char *att);
        GLint getUniformLocation(const char *uni);

        bool isCompiled() { return compiled; }

        void bind();
        void unbind();
    };
}

#endif
