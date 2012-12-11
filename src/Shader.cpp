#include <fstream>
#include <cstdio>
#include <cstring>
#include "Shader.h"
#include "Engine.h"
#include "Log.h"

#define BUFFER_SIZE 2048

/**
 * readTextFile - reads a simple textfile specified by filename,
 * if everything is ok, a pointer to a null-terminated string is returned,
 * otherwise NULL is returned.
 */

char* readTextFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if(file == NULL)
    {
      logError("Cannot open file %s", filename);
      return 0;
    }

    fseek(file, 0, SEEK_END);

    char *buffer = NULL;
    int bytesinfile = ftell(file);
    rewind(file);

    if(bytesinfile > 0)
    {
        buffer = (char*)malloc(bytesinfile+1);
        int bytesread = fread( buffer, 1, bytesinfile, file);
        buffer[bytesread] = '\0'; // Terminate the string with a null character
    }

    fclose(file);

    return buffer;
}

Shader::Shader()
{
    vertexShader    = 0;
    fragmentShader  = 0;
    program         = 0;
    compiled        = 0;

    positionLoc     = -1;
    normalLoc       = -1;
    texCoordLoc     = -1;
    viewMatrixLoc   = -1;
    projMatrixLoc   = -1;

    vertexFile      = NULL;
    fragmentFile    = NULL;
}

Shader::Shader( const char *vertFile, const char *fragFile )
{
    vertexShader    = 0;
    fragmentShader  = 0;
    program         = 0;
    compiled        = 0;

    positionLoc     = -1;
    normalLoc       = -1;
    texCoordLoc     = -1;
    viewMatrixLoc   = -1;
    projMatrixLoc   = -1;

    vertexFile      = NULL;
    fragmentFile    = NULL; 

    setVertexFile(vertFile);
    setFragmentFile(fragFile);
    loadAndCompile();
}

Shader::~Shader()
{
    if(vertexFile)
        delete vertexFile;

    if(fragmentFile)
        delete fragmentFile;

    glDetachShader(program, fragmentShader); // Detach the fragment shader
    glDetachShader(program, vertexShader); // Detach the vertex shader

    glDeleteShader(fragmentShader); // Delete the fragment shader
    glDeleteShader(vertexShader); // Delete the vertex shader
    glDeleteProgram(program); // Delete the shader program
}

void Shader::setVertexFile(const char *vertFile)
{
    if(!vertFile)
    {
        logError("A null-pointer was passed");
        return;
    }

    if(vertexFile)
        delete vertexFile;

    int len = (int)strlen(vertFile);

    vertexFile = new char[len+1];
    strcpy(vertexFile, vertFile);
    vertexFile[len] = '\0';
}

void Shader::setFragmentFile(const char *fragFile)
{
    if(!fragFile)
    {
        logError("A null-pointer was passed");
        return;
    }

    if(fragmentFile)
        delete fragmentFile;

    int len = (int)strlen(fragFile);

    fragmentFile = new char[len+1];
    strcpy(fragmentFile, fragFile);
    fragmentFile[len] = '\0';
}

void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
 
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
 
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n",infoLog);
        free(infoLog);
    }
}
 
void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
 
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
 
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n",infoLog);
        free(infoLog);
    }
}

/*
 * loadAndCompile - create, load, compile and link a shader object.
 */
bool Shader::loadAndCompile()
{
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
 
    char *vs = readTextFile(vertexFile);
    char *fs = readTextFile(fragmentFile);
 
    const char * vv = vs;
    const char * ff = fs;
 
    glShaderSource(vertexShader, 1, &vv, NULL);
    glShaderSource(fragmentShader, 1, &ff, NULL);
 
    free(vs);
    free(fs);
 
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
 
    printShaderInfoLog(vertexShader);
    printShaderInfoLog(fragmentShader);

    program = glCreateProgram();
    glAttachShader(program,vertexShader);
    glAttachShader(program,fragmentShader);
 
    //glBindFragDataLocation(program, 0, "outputF");
    glBindAttribLocation(program, 0, "in_position");
    glBindAttribLocation(program, 1, "in_normal");
    glBindAttribLocation(program, 2, "in_texCoord");

    glLinkProgram(program);
    printProgramInfoLog(program);
 
    positionLoc = glGetAttribLocation(program,"in_position");
    normalLoc = glGetAttribLocation(program, "in_normal");
    texCoordLoc = glGetAttribLocation(program, "in_texCoord");

    logNote("programId: %i, posLoc %i, normLoc %i, texLoc %i", program, positionLoc, normalLoc, texCoordLoc);
 
    projMatrixLoc = glGetUniformLocation(program, "projMatrix");
    viewMatrixLoc = glGetUniformLocation(program, "viewMatrix");
    modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");

    compiled = true;
 
    return true;
}

GLint Shader::getAttributeLocation(const char *att)
{
    return glGetAttribLocation(program, att);
}


GLint Shader::getUniformLocation(const char *uni)
{
    return glGetUniformLocation(program, uni);
}

void Shader::bind()
{
    if(compiled)
        glUseProgram(program);
}

void Shader::unbind()
{
    glUseProgram(0);
}