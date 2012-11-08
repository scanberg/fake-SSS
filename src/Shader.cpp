#include <fstream>
#include <cstdio>
#include <string>
#include "Shader.h"
#include "Log.h"

#define BUFFER_SIZE 512

/*
 * filelength - Determine the number of bytes in a file.
 * This is a lazy hack to avoid calling stat(), but it works. /STEGU
 */
static int filelength( const char *filename ) {
    FILE *ifp;
    int length;

    ifp = fopen( filename, "r" );
    fseek( ifp, 0, SEEK_END );
    length = (int)ftell( ifp );
    fclose( ifp );
    return length;
}

static char *readTextFile(const char *filename)
{
    FILE *file = fopen( filename, "r" );
    if( file == NULL )
    {
        logError( "Cannot open shader file!" );
        return 0;
    }
    int bytesinfile = filelength( filename );
    char *buffer = (char*)malloc( bytesinfile+1 );
    int bytesread = fread( buffer, 1, bytesinfile, file );
    buffer[bytesread] = 0; // Terminate the string with 0
    fclose( file );

    return buffer;
}

/**
    Given a shader and the filename associated with it, validateShader will
    then get information from OpenGl on whether or not the shader was compiled successfully
    and if it wasn't, it will output the file with the problem, as well as the problem.
*/
static void validateShader(GLuint shader) {
    char buffer[BUFFER_SIZE];
    GLsizei length = 0;

    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the shader
    if (length > 11) // If we have any information to display
        std::cout << "Shader " << shader << " compile error: " << buffer << std::endl; // Output the information
}

/**
    Given a shader program, validateProgram will request from OpenGL, any information
    related to the validation or linking of the program with it's attached shaders. It will
    then output any issues that have occurred.
*/
static void validateProgram(GLuint program) {
    char buffer[BUFFER_SIZE];
    GLsizei length = 0;

    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the program
    if (length > 11) // If we have any information to display
        printf("Program link error(s): \n%s\n",buffer); // Output the information

    glValidateProgram(program); // Get OpenGL to try validating the program
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status); // Find out if the shader program validated correctly
    if (status == GL_FALSE) // If there was a problem validating
        printf("Error validating program \n");
}

Shader::Shader()
{
    compiled=false;
}

Shader::Shader( const char *vertexShader, const char *fragmentShader )
{
    compiled = false;
    init(vertexShader, fragmentShader);
}

Shader::~Shader() {
    glDetachShader(program, fragmentShader); // Detach the fragment shader
    glDetachShader(program, vertexShader); // Detach the vertex shader

    glDeleteShader(fragmentShader); // Delete the fragment shader
    glDeleteShader(vertexShader); // Delete the vertex shader
    glDeleteProgram(program); // Delete the shader program
}

void Shader::init(const char *vertexFile, const char *fragmentFile) {
    if (compiled) // If we have already initialized the shader
        return;

    logNote("Building shader from %s & %s", vertexFile, fragmentFile);

    vertexShader = glCreateShader(GL_VERTEX_SHADER); // Create a vertex shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Create a fragment shader

    const char *vertexText = readTextFile(vertexFile);
    const char *fragmentText = readTextFile(fragmentFile);

    if (!vertexText || !fragmentText)
    {
        logError("unable to load shaderFile or fragmentFile");

        delete[] vertexText;
        delete[] fragmentText;
    }

    glShaderSource(vertexShader, 1, &vertexText, 0); // Set the source for the vertex shader to the loaded text
    glCompileShader(vertexShader); // Compile the vertex shader
    validateShader(vertexShader); // Validate the vertex shader

    glShaderSource(fragmentShader, 1, &fragmentText, 0); // Set the source for the fragment shader to the loaded text
    glCompileShader(fragmentShader); // Compile the fragment shader
    validateShader(fragmentShader); // Validate the fragment shader

    program = glCreateProgram(); // Create a GLSL program
    glAttachShader(program, vertexShader); // Attach a vertex shader to the program
    glAttachShader(program, fragmentShader); // Attach the fragment shader to the program

    glBindAttribLocation(program, 0, "in_position"); // Bind a constant attribute location for positions of vertices
    glBindAttribLocation(program, 1, "in_normal"); // Bind another constant attribute location, this time for color
    glBindAttribLocation(program, 2, "in_texCoord");
    glBindAttribLocation(program, 3, "in_tangent");
    glBindAttribLocation(program, 4, "in_color");

    //glBindFragDataLocation(programId, 0, "myVec41");
    //glBindFragDataLocation(programId, 1, "myVec42");

    glLinkProgram(program); // Link the vertex and fragment shaders in the program
    validateProgram(program); // Validate the shader program

    GLint isLinked;

    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if(isLinked==GL_FALSE)
    {
        logError("Failed to link shader");

        GLsizei length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        if(length>0)
        {
            char buffer[BUFFER_SIZE];
            glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
            logNote(buffer);
        }

        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);
        glDeleteShader(vertexShader);
        vertexShader=0;
        glDeleteShader(fragmentShader);
        fragmentShader=0;
        glDeleteProgram(program);
        program=0;

        delete[] vertexText;
        delete[] fragmentText;

        return;
    }

    compiled = true; // Mark that we have initialized the shader

    std::cout<<"Shader compiled successfully"<<std::endl;
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
