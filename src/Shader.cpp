#include <fstream>
#include <cstdio>
#include <cstring>
#include "Shader.h"
#include "Log.h"

#define BUFFER_SIZE 512

namespace glen
{
    /**
     * filelength - Determine the number of bytes in a file.
     * This is a lazy hack to avoid calling stat(), but it works. /STEGU
     */
    static int filelength( FILE *ifp )
    {
        if(!ifp)
        {
            logError("ifp was passed as NULL");
            return 0;
        }
        int length;

        //ifp = fopen( filename, "r" );
        fseek( ifp, 0, SEEK_END );
        length = (int)ftell( ifp );
        fseek( ifp, 0, SEEK_SET );
        //fclose( ifp );
        return length;
    }
    /**
     * readTextFile - reads a simple textfile specified by filename,
     * if everything is ok, a pointer to a null-terminated string is returned,
     * otherwise NULL is returned.
     */
    static char *readTextFile(const char *filename)
    {
        FILE *file = fopen( filename, "r" );
        //logNote("file is %s",filename);
        if( file == NULL )
        {
            logError( "Cannot open shader file!" );
            return NULL;
        }
        int bytesinfile = filelength( file );
        char *buffer = (char*)malloc( bytesinfile+1 );
        int bytesread = fread( buffer, 1, bytesinfile, file );
        //logNote("Bytesread %i", bytesread);
        buffer[bytesread] = 0; // Terminate the string with 0
        fclose( file );

        //logNote("buffer: %s",buffer);

        return buffer;
    }

    /**
     * Given a shader and the filename associated with it, validateShader will
     * then get information from OpenGl on whether or not the shader was compiled successfully
     * and if it wasn't, it will output the file with the problem, as well as the problem.
     */
    static bool validateShader(GLuint shader)
    {
        char buffer[BUFFER_SIZE];
        GLsizei length = 0;

        glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the shader
        if (length > 11) // If we have any information to display
        {
            logError("Shader compile error(s): \n%s",buffer);
            return false;
        }

        return true;
    }

    /**
        Given a shader program, validateProgram will request from OpenGL, any information
        related to the validation or linking of the program with it's attached shaders. It will
        then output any issues that have occurred.
    */
    static bool validateProgram(GLuint program)
    {
        glValidateProgram(program); // Get OpenGL to try validating the program
        GLint status;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &status); // Find out if the shader program validated correctly
        
        if (status == GL_FALSE) // If there was a problem validating
        {
            char buffer[BUFFER_SIZE];
            GLsizei length = 0;

            glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the program
            if (length > 11) // If we have any information to display
                logError("Program link error(s): \n%s",buffer); // Output the information
            
            return false;
        }

        return true;
    }

    Shader::Shader()
    {
        vertexShader    = 0;
        fragmentShader  = 0;
        program         = 0;
        compiled        = 0;
        vertexFile      = NULL;
        fragmentFile    = NULL;
    }

    Shader::Shader( const char *vertFile, const char *fragFile )
    {
        vertexShader    = 0;
        fragmentShader  = 0;
        program         = 0;
        compiled        = 0;
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
        //vertexFile[len] = 0;
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
        strcpy(fragmentFile, fragFile); //, len);
        //fragmentFile[len] = 0;
    }

    bool Shader::loadAndCompile()
    {
        logNote("IN LOAD AND COMPILE");
        if(!vertexFile || !fragmentFile)
        {
            logError("VertexFile or FragmentFile not specified");
            return false;
        }

        logNote("Building shader from vs: \"%s\" and fs: \"%s\"", vertexFile, fragmentFile);

        vertexShader = glCreateShader(GL_VERTEX_SHADER); // Create a vertex shader
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Create a fragment shader

        if(!vertexShader || !fragmentShader)
        {
            logError("OpenGL could not to create vertex- or fragment- shader");
            return false;
        }

        const char *vertexText = readTextFile(vertexFile);
        const char *fragmentText = readTextFile(fragmentFile);

        if (!vertexText || !fragmentText)
        {
            logError("unable to load shaderFile or fragmentFile");

            delete vertexText;
            delete fragmentText;
            return false;
        }

        glShaderSource(vertexShader, 1, &vertexText, 0); // Set the source for the vertex shader to the loaded text
        glCompileShader(vertexShader); // Compile the vertex shader
        delete vertexText;

        validateShader(vertexShader); // Validate the vertex shader

        glShaderSource(fragmentShader, 1, &fragmentText, 0); // Set the source for the fragment shader to the loaded text
        glCompileShader(fragmentShader); // Compile the fragment shader
        delete fragmentText;
        
        validateShader(fragmentShader); // Validate the fragment shader
        
        program = glCreateProgram(); // Create a GLSL program
        glAttachShader(program, vertexShader); // Attach a vertex shader to the program
        glAttachShader(program, fragmentShader); // Attach the fragment shader to the program

        glBindAttribLocation(program, 0, "in_position"); // Bind a constant attribute location for positions of vertices
        //glBindAttribLocation(program, 1, "in_normal"); // Bind another constant attribute location, this time for color
        //glBindAttribLocation(program, 2, "in_texCoord");
        //glBindAttribLocation(program, 3, "in_tangent");
        //glBindAttribLocation(program, 4, "in_color");

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
                logNote("%s",buffer);
            }

            glDetachShader(program, vertexShader);
            glDetachShader(program, fragmentShader);

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(program);

            fragmentShader=0;
            vertexShader=0;
            program=0;

            return false;
        }

        compiled = true; // Mark that we have successfully compiled the shader

        logNote("Shader compiled successfully.");

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

}
